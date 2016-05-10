#include "HTTPServer.hpp"
#include <thread>
#include <sstream>
#include <functional>


HTTPServer::HTTPServer()
{
	Configuration configuration = Configuration::getInstance();
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
		std::thread thread(&HTTPServer::response, this, std::ref(socket));
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
	size_t f = response.find("{0}");
	response.replace(f, std::string("{0}").length(), std::to_string(response_json.size()));
	response = response + "\r\n" + response_json;
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

