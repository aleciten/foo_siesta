#include "stdafx.h"
#include "FooPlayControl.h"
#include "FooMainThreadRunner.h"
#include "SDK/foobar2000.h"
#include "helpers/helpers.h"
#include <functional>
#include <future>

FooPlayControl::FooPlayControl()
{
	
}

void FooPlayControl::play() {	
	auto f = MainThreadRunner<int>([]() -> int {
		static_api_ptr_t<playback_control> pc;
		pc->start();
		return 0;
	}).run();

    f.wait();
    f.get();
}

void FooPlayControl::stop() {
	MainThreadRunner<int>([]() -> int {
		static_api_ptr_t<playback_control> pc;
		pc->stop();
		return 0;
	}).run();
}

void FooPlayControl::pause() {
	MainThreadRunner<int>([]() -> int {
		static_api_ptr_t<playback_control> pc;
		pc->pause(true);
		return 0;
	}).run();
}

void FooPlayControl::nextTrack() {
	MainThreadRunner<int>([]() -> int {
		static_api_ptr_t<playback_control> pc;
		pc->start(playback_control::track_command_next);
		return 0;
	}).run();
}

void FooPlayControl::prevTrack() {
	MainThreadRunner<int>([]() -> int {
		static_api_ptr_t<playback_control> pc;
		pc->start(playback_control::track_command_prev);
		return 0;
	}).run();
}

void FooPlayControl::randTrack() {
	MainThreadRunner<int>([]() -> int {
		static_api_ptr_t<playback_control> pc;
		pc->start(playback_control::track_command_rand);
		return 0;
	}).run();
}