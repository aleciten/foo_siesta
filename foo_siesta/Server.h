#pragma once
#include <memory>
#include <string>

// Forward declaration to avoid including Poco's full header
// Poco uses Winsock2 and will clash with foobar's SDK which uses version 1
namespace Poco {
	namespace Net {
		class HTTPServer;
	};
};

class Server {
public:
	Server(UInt16 port, std::string baseUrl);
	Server(UInt16 port);
	~Server();

	void Start();
	void Stop();
	
private:
	UInt16 m_port;
	std::string m_baseUrl;
	Poco::Net::HTTPServer *m_server;
};