#include "stdafx.h"

std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

bool ws2i(const std::wstring& wstr, int &out) {
	std::wstringstream ss;
	ss << wstr;
	int i;
	ss >> i;

	if (ss.fail()) return false;
	out = i;

	return true;
}

pfc::string8 ws2s8(const string_t &wstr) {
	return pfc::string8(string(wstr.begin(), wstr.end()).c_str());
}