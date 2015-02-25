#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class BaseRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:	
	BaseRequestHandler(std::vector<std::string> relativeSegments) : _relativeSegments(relativeSegments)
	{
	}

	virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) = 0;

protected:
	std::vector<std::string> _relativeSegments;
};

class PlayControlRequestHandler : public BaseRequestHandler
{
public:
	PlayControlRequestHandler(std::vector<std::string> relativeSegments) : BaseRequestHandler(relativeSegments)
	{
	}

	void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);

private:
	static int count;
};

class PlaylistsRequestHandler : public BaseRequestHandler
{
public:
	PlaylistsRequestHandler(std::vector<std::string> relativeSegments) : BaseRequestHandler(relativeSegments)
	{
	}
	void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);

private:
	static int count;
};

class UserInterfaceRequestHandler : public BaseRequestHandler
{
public:
    UserInterfaceRequestHandler(std::vector<std::string> relativeSegments) : BaseRequestHandler(relativeSegments) { }
    void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);

private:
    static int count;
};