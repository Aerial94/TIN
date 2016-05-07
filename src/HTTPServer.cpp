#include "HTTPServer.hpp"
#include <thread>
#include <sstream>
#include <iostream>
#include <string>

HTTPServer::HTTPServer()
{
	SocketAddress address;
	address.setAddress(configuration.getHttpServerAddress());
	address.setPort(configuration.getHttpServerPort());
	this->socket.bind(address);
	this->socket.listen(configuration.getHttpServerMaxThreads());
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

void HTTPServer::response(TCPSocket& socekt)
{
	std::string http_request, respone;
	socket >> http_request;
	if (!this->is_request_valid(http_request))
	{
		respone = this->invalid_request(socket);
	}
	else
	{
		std::string json = this->get_json(http_request);
		std::string response = hander.getResponse(json);
		response = this->valid_request(socekt, response);
	}
	socket << respone;
}

std::string HTTPServer::get_json(std::string & request)
{
	std::string line, json;
	std::istringstream iss(temp);
	while(getline(iss,line) && !line.empty());
	while(getline(iss,line)){
		json += line;
	}
	return json;
}

std::string HTTPServer::valid_request(std::string& respone_json)
{
	std::string respone(this->valid_request);
	respone = respone + "\n\n" + respone_json;
	return respone;
}

std::string HTTPServer::invalid_request()
{
	std::string respone(this->invalid_request);
	return respone

}

bool HTTPServer::is_request_valid(std::string &request)
{
    return (request.find("POST") != std::string::npos);
}

