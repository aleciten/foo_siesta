#include "stdafx.h"
#include "component.h"
#include <future>
#include "help.h"
#include "resource.h"

using http::methods;
using http::status_codes;
using std::function;

bool ui_component::handle(std::vector<string_t> paths, http::http_request msg) {
	if (msg.method() != methods::GET) return false;
	return get(msg);
}

wstring get_resource_wstring(int name)
{
	HMODULE handle = ::GetModuleHandle(L"foo_webservice.dll");
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(23));
	HGLOBAL rcData = ::LoadResource(handle, rc);
	auto data = static_cast<const char*>(::LockResource(rcData));
	
	return s2ws(data);
}

bool ui_component::get(http::http_request msg) {
	auto html = get_resource_wstring(IDR_MINISITE_HTML);
	
	msg.reply(status_codes::OK, html, L"text/html");
	return false;
}