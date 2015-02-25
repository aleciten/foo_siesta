#include "stdafx.h"
#include <algorithm>
#include <Poco\Net\HTTPRequestHandlerFactory.h>
#include <Poco\Net\HTTPServerRequest.h>
#include <Poco\URI.h>

#include "RequestHandlerFactory.h"
#include "RequestHandlers.h"

using namespace Poco::Net;
using std::string;
using std::vector;

RequestHandlerFactory::RequestHandlerFactory(std::string baseUrl)
{
	Poco::URI(baseUrl).getPathSegments(_basePathSegments);

	// relative to base
	_registerHandler<UserInterfaceRequestHandler>("/");
	_registerHandler<PlayControlRequestHandler>("control");
	_registerHandler<PlaylistsRequestHandler>("playlists");
}

RequestHandlerFactory::RequestHandlerFactory() : RequestHandlerFactory("/")
{
}

Poco::Net::HTTPRequestHandler* RequestHandlerFactory::_create(std::string key, std::vector<std::string> relativeSegments) {
	if (_handlerRegistry.find(key) == _handlerRegistry.end())
		return NULL;

	return _handlerRegistry[key](relativeSegments);
}

bool RequestHandlerFactory::_matchesBase(Poco::URI &uri) {
	vector<string> segments;
	uri.getPathSegments(segments);
		
	if (segments.size() < _basePathSegments.size()) {
		return false;
	}
	else if (segments.size() > _basePathSegments.size()) {
		segments = vector<string>(segments.begin(), segments.begin() + _basePathSegments.size());
	}

	return segments == _basePathSegments;
}

HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request)
{
	auto uriString = request.getURI();
	std::transform(uriString.begin(), uriString.end(), uriString.begin(), tolower);
	auto uri = Poco::URI(uriString);
	
	if (!_matchesBase(uri)) return NULL;
	
	vector<string> segments;
	uri.getPathSegments(segments);
	
	auto relativeSegments = vector<string>(segments.begin() + _basePathSegments.size(), segments.end());
    auto handlerKey = relativeSegments.size() > 0 ? relativeSegments[0] : "/"; 

	return _create(handlerKey, vector<string>(relativeSegments.begin()+ (relativeSegments.size() > 0 ? 1 : 0), relativeSegments.end()));
}
