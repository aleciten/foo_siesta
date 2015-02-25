#include "stdafx.h"
#include "RequestHandlers.h"
#include "resource.h"
#include <windows.h>

std::string getResourceString(int name)
{
    HMODULE handle = GetModuleHandle(L"foo_siesta.dll");
    HRSRC rc = FindResourceW(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(23));
    HGLOBAL rcData = LoadResource(handle, rc);
    auto data = static_cast<const char*>(LockResource(rcData));
    return data;
}

void UserInterfaceRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
{
    resp.setContentType("text/html");
    std::ostream& out = resp.send();

    out << getResourceString(IDR_UI_HTML);
    out.flush();
}