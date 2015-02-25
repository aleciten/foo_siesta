#include "stdafx.h"
#include "RequestHandlers.h"
#include "FooPlayControl.h"

using namespace Poco::Net;

void PlayControlRequestHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
{
	resp.setContentType("text/html");

	//std::ostream& out = resp.send();
		
	FooPlayControl pc;
	auto rs = _relativeSegments;
	
    if (req.getMethod() == "POST") 
    {
	    if      (rs.empty()) { }
	    else if (rs[0] == "play")  pc.play();
	    else if (rs[0] == "stop")  pc.stop();
	    else if (rs[0] == "pause") pc.pause();
	    else if (rs[0] == "next")  pc.nextTrack();
	    else if (rs[0] == "prev")  pc.prevTrack();
	    else if (rs[0] == "rand")  pc.randTrack();
	    else 
        {
		    resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
	    }
    }
    else 
    {
        resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    }

	resp.send();// .flush();
}