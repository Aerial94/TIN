#include "HTTPServer.hpp"
#include <thread>
#include <sstream>


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
		std::thread thread(this);
		thread.detach();
	}
}

void HTTPServer::response(TCPSocket& socket)
{
	std::string http_request, response;
	socket >> http_request;
	if (!this->is_request_valid(http_request))
	{
		response = this->invalid_request_function();
	}
	else
	{
		std::string json = this->get_json(http_request);
		std::string jsonResponse = handler.getResponse(json);
		response = this->valid_request_function(jsonResponse);
	}
	socket << response;
}

std::string HTTPServer::get_json(std::string & request)
{
	std::string line, json;
	std::istringstream iss(request);
	while(getline(iss,line) && !line.empty());
	while(getline(iss,line)){
		json += line;
	}
	return json;
}

std::string HTTPServer::valid_request_function(std::string &response_json)
{
	std::string response(this->valid_request);
	response = response + "\n\n" + response_json;
	return response;
}

std::string HTTPServer::invalid_request_function()
{
	std::string response(this->invalid_request);
	return response;

}

bool HTTPServer::is_request_valid(std::string &request)
{
    return (request.find("POST") != std::string::npos);
}

