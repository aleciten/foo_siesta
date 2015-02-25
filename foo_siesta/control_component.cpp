#include "stdafx.h"
#include "component.h"

using http::methods;
using http::status_codes;
using std::function;

bool control_component::handle(std::vector<string_t> paths, http::http_request msg) {
	auto method = msg.method();
	
	if (paths.empty()) return false;
	auto action = paths[0];

	if (method == methods::GET) {
		if (action == L"play")        return play(playback_control::track_command_play, msg);
		else if (action == L"next")   return play(playback_control::track_command_next, msg);
		else if (action == L"prev")   return play(playback_control::track_command_prev, msg);
		else if (action == L"random") return play(playback_control::track_command_rand, msg);
		else if (action == L"stop")   return stop(msg);
		else if (action == L"pause")  return pause(msg);
		else if (action == L"vol")    return get_vol(msg);
	}

	return false;
}

bool control_component::play(playback_control::t_track_command cmd, http::http_request msg) {
	pc_do(std::function<bool(pc_ptr)>([cmd](pc_ptr pc) -> bool {
		pc->start(cmd);
		return true;
	}));
	
	msg.reply(status_codes::OK, L"");
	return true;
}

bool control_component::stop(http::http_request msg) {
	pc_do(std::function<bool(pc_ptr)>([](pc_ptr pc) -> bool {
		pc->stop();
		return true;
	}));

	msg.reply(status_codes::OK, L"");
	return true;
}

bool control_component::pause(http::http_request msg) {
	pc_do(std::function<bool(pc_ptr)>([](pc_ptr pc) -> bool {
		pc->pause(!pc->is_paused());
		return true;
	}));

	msg.reply(status_codes::OK, L"");
	return true;
}

bool control_component::set_vol(float vol, http::http_request msg) {
	pc_do(std::function<bool(pc_ptr)>([vol](pc_ptr pc) -> bool {
		pc->set_volume(vol);
		return true;
	}));

	msg.reply(status_codes::OK, json::value());
	return true;
}

bool control_component::get_vol(http::http_request msg) {
	auto vol = pc_do(std::function<float(pc_ptr)>([](pc_ptr pc) -> float {
		return pc->get_volume();
	}));

	json::value ret;
	ret[L"vol"] = json::value(vol);
	msg.reply(status_codes::OK, ret);
	return true;
}
