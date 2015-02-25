#pragma once
#include "stdafx.h"

extern std::wstring s2ws(const std::string& str);
extern bool ws2i(const std::wstring& wstr, int &out);
extern pfc::string8 ws2s8(const std::wstring& wstr);

