#include "stdafx.h"
#include "RequestHandlers.h"
#include "FooPlaylists.h"

using namespace Poco::Net;

void PlaylistsRequestHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
{
    resp.setContentType("text/html");
    auto &os = resp.send();

    //std::ostream& out = resp.send();

    FooPlaylists pc;
    auto rs = _relativeSegments;

    if (req.getMethod() == "GET")
    {
        FooPlaylistInfo info;
        pc.getPlaylistInfo(pc.getActive(), info);
        os << info.name;
    }
    else
    {
        resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    }

    os.flush();
}