#include "HTTPServer.hpp"
#include "HTTPPacket.hpp"
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
	HTTPPacket packet;
	std::string json_response, response;
	std::string line, json;
	line = socket.readLine();
	while(!line.compare("\r\n")){
		packet.save_line(line);
		line = socket.readLine();
	}
	json = socket.read_from_socket(packet.get_content_length());
	json_response = handler.getResponse(json);
	response = valid_request_function(json_response);
	socket << response;
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

