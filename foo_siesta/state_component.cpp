#include "stdafx.h"
#include "component.h"

using http::methods;
using http::status_codes;
using std::function;

bool state_component::handle(std::vector<string_t> paths, http::http_request msg) {
	if (!paths.empty()) return false;

	if (msg.method() == methods::GET) {
		return get_state(msg);
	}

	return false;
}

bool state_component::get_state(http::http_request msg)
{
	auto ret = in_main_thread(function<json::value(void)>([]() -> json::value {
		json::value ret;
		static_api_ptr_t<play_control> pc;
		static_api_ptr_t<playlist_manager> pm;
		
		auto order_index = pm->playback_order_get_active();
		string order_name(pm->playback_order_get_name(order_index));
		ret[L"playback_order"] = json::value(s2ws(order_name));

		ret[L"stop_after_current"] = json::value(pc->get_stop_after_current());

		json::value state;
		if      (pc->is_paused())  state = json::value(L"paused");
		else if (pc->is_playing()) state = json::value(L"playing");
		else                       state = json::value(L"stopped");
		ret[L"state"] = state;

		json::value curr_track;
		metadb_handle_ptr handle;
		if (pc->get_now_playing(handle)) {
			service_ptr_t<titleformat_object> script;
			static_api_ptr_t<titleformat_compiler>()->compile_safe(script, "%album artist% - %album% - %title%");

			pfc::string_formatter text;
			pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
			
			curr_track[L"name"] = json::value(s2ws(text.get_ptr()));
			curr_track[L"length"] = json::value(pc->playback_get_length());
			curr_track[L"pos"] = json::value(pc->playback_get_position());
			curr_track[L"can_seek"] = json::value(pc->playback_can_seek());
		}
		ret[L"current_track"] = curr_track;

		int active_pl = -1;
		active_pl = pm->get_active_playlist();
		if (active_pl >= 0) {
			pfc::string8 pl_name;
			pm->playlist_get_name(active_pl, pl_name);
			json::value playlist;
			playlist[L"index"] = json::value(active_pl);
			playlist[L"name"] = json::value(s2ws(pl_name.get_ptr()));
			ret[L"active_playlist"] = playlist;
		}

		return ret;
	}));

	msg.reply(status_codes::OK, ret);
	return true;
}
