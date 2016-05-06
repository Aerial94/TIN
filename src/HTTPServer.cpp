#include "HTTPServer.hpp"
#include <thread>

HTTPServer::HTTPServer()
{
	SocketAddress address;
	address.setAddress(configuration.getHttpServerAddress);
	address.setPort(configuration.getHttpServerPort);
	this->socket.bind(address);
	this->socket.listen(configuration.getHttpServerMaxThreads);
}

void HTTPServer::listen()
{
	while (1) 
	{
		TCPSocket socket = this->socket.accept();
		std::thread thread(&HTTPServer::response, this, socket);
		thread.detach();
	}
}

void HTTPServer::response(TCPSocket socekt)
{
	std::string http_request;
	socket >> http_request;
	if (!this->is_request_valid(http_request))
	{
		this->invalid_request(socket);
	}
	else
	{
		std::string json = this->get_json(http_request);
		std::string response = hander.testAction(json);
		this->valid_request(socekt, response);
	}
}

bool HTTPServer::get_json(std::string request)
{

}

void HTTPServer::valid_request(TCPSocket socket, std::string respone_json)
{

}

void HTTPServer::invalid_request(TCPSocket socket)
{

}