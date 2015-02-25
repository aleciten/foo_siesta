#pragma once
#include "stdafx.h"
#include <string>

struct FooPlaylistInfo {
    int index;
    std::string name;
};

class FooPlaylists
{
public:
    FooPlaylists();
    ~FooPlaylists();

    int getActive();
    //void setActive(int index);    
    //void getNames(int index);
    void getPlaylistInfo(int index, FooPlaylistInfo &info);
};

