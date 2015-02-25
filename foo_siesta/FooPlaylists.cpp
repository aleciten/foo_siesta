#include "stdafx.h"
#include "FooPlaylists.h"
#include "FooMainThreadRunner.h"
#include "SDK/foobar2000.h"
#include "helpers/helpers.h"
#include <functional>
#include <future>

FooPlaylists::FooPlaylists()
{
}

FooPlaylists::~FooPlaylists()
{
}

void FooPlaylists::getPlaylistInfo(int index, FooPlaylistInfo &info)
{
    auto future = MainThreadRunner<int>([index, &info]() -> int {
        static_api_ptr_t<playlist_manager> pm;
        pfc::string8 out;
        pm->playlist_get_name(index, out);
        
        info.index = index;
        info.name = out;
        return 0;
    }).run();
    future.wait();
}

int FooPlaylists::getActive() 
{
    auto future = MainThreadRunner<int>([]() -> int {
        static_api_ptr_t<playlist_manager> pm;
        auto index = pm->get_active_playlist();
        return index;
    }).run();

    future.wait();
    return future.get();
}
