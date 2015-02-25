#include "stdafx.h"
#include "Server.h"
#include "RequestHandlerFactory.h"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>

using namespace Poco::Net;


Server::Server(UInt16 port, std::string baseUrl) :
	m_server(NULL),
	m_baseUrl(baseUrl),
	m_port(port)
{
}

Server::Server(UInt16 port) : Server(port, "/")
{
}

Server::~Server() {
	if (m_server != NULL)
		delete m_server;
}

void Server::Start() {
	auto params = new HTTPServerParams;
	params->setMaxQueued(100);
	params->setMaxThreads(4);
	
	ServerSocket svs(m_port);
	
	m_server = new HTTPServer(new RequestHandlerFactory(m_baseUrl), svs, params);
	m_server->start();
}

void Server::Stop() {
	if (m_server == NULL) return;

	m_server->stop();
	m_server->stopAll();
	delete m_server;
}