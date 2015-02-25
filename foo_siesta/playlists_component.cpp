#include "stdafx.h"
#include "component.h"
#include <future>
#include "help.h"

using http::methods;
using http::status_codes;
using std::function;

std::map<string_t, string_t> extract_query_map(http::http_request msg) {
	auto uri = msg.relative_uri();
	auto query = http::uri::split_query(uri.query());
	return query;
}

bool playlists_component::handle(std::vector<string_t> paths, http::http_request msg) {
	auto method = msg.method();
	auto query = extract_query_map(msg);

	if (paths.empty()) {
		if (method == methods::GET) {			
			return get_all(true, msg);
		}
	}
	
	int i;
	auto is_number = ws2i(paths[0], i);
	
	if (msg.method() == methods::GET) {
		if (is_number) {
			return get_index(i, msg);
		}
		else {
			if (paths[0] == L"active") 
				return get_active(msg);
		}
	}

	return false;
}

string_t format(metadb_handle_ptr handle, pfc::string8 format){
	static_api_ptr_t<play_control> pc;
	service_ptr_t<titleformat_object> script;
	static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);
	
	pfc::string_formatter text;
	pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
	return s2ws(text.get_ptr());
}

json::value get_playlist_info(size_t index, bool include_contents = true) {
	auto info = json::value();

	static_api_ptr_t<playlist_manager> pm;
	info[L"index"] = json::value(index);

	pfc::string8 name;
	pm->playlist_get_name(index, name);
	info[L"name"] = json::value(s2ws(name.get_ptr()));

	auto contents = json::value();
	metadb_handle_list mdb_list;
	pm->playlist_get_all_items(index, mdb_list);

	info[L"total_duration"] = json::value(mdb_list.calc_total_duration());

	if (include_contents) {
		auto item_count = mdb_list.get_size();
		for (auto i = 0; i < item_count; i++) {
			auto item = mdb_list.get_item(i);
			auto entry = json::value();

			entry[L"artist"] = json::value(format(item, "%artist%"));
			entry[L"title"] = json::value(format(item, "%title%"));

			contents[i] = entry;
		}
		info[L"contents"] = contents;
	}

	return info;
}

bool playlists_component::get_all(bool include_contents, http::http_request msg) {	
	auto ret = in_main_thread(function<json::value()>([include_contents]() -> json::value {
		static_api_ptr_t<playlist_manager> pm;
		auto result = json::value::array();

		auto item_count = pm->get_playlist_count();
		if (item_count == 0) return result;

		for (t_size i = 0; i < item_count; i++) {
			result[i] = get_playlist_info(i, include_contents);
		}

		return result;
	}));
	
	msg.reply(status_codes::OK, ret);
	return true;
}

bool playlists_component::get_index(int index, http::http_request msg) {
	auto ret = in_main_thread(function<json::value()>([index]() -> json::value {
		static_api_ptr_t<playlist_manager> pm;
		auto count = pm->get_playlist_count();
		if (index < 0 || index >= count) return NULL;

		auto result = get_playlist_info(index);
		return result;
	}));
	
	if (ret == NULL) return false;
	msg.reply(status_codes::OK, ret);
	return true;
}

bool playlists_component::get_active(http::http_request msg) {
	auto ret = in_main_thread(function<json::value()>([]() -> json::value {
		static_api_ptr_t<playlist_manager> pm;
		int active_idx = -1;
		active_idx = pm->get_active_playlist();
		
		if (active_idx < 0) return NULL; 
		else return get_playlist_info(active_idx);
	}));

	if (ret == NULL) return false;
	msg.reply(status_codes::OK, ret);
	return true;
}