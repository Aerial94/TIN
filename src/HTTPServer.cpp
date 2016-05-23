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
    this->socket.setReuseAddress();
	this->socket.bind(address);
	this->socket.listen(configuration.getHttpServerMaxThreads());
}

void HTTPServer::listen()
{
	while (1) 
	{
		int clientSocket = this->socket.accept();
		std::thread thread(&HTTPServer::response, this, clientSocket);
		thread.detach();
	}
}

void HTTPServer::response(int clientSocket)
{
	TCPSocket tcpSocket(clientSocket);
	tcpSocket.setTimeout(Configuration::getInstance().getHttpServerReadTimeout(),0);
	HTTPPacket packet;
	std::string json_response, response;
	std::string line, json;
	line = tcpSocket.readLine();
	while(line != "\r\n"){
		packet.save_line(line);
		line = tcpSocket.readLine();
	}
	json = tcpSocket.read_from_socket(packet.get_content_length());
    HTTPHandler handler;
	json_response = handler.getResponse(json);
	response = valid_request_function(json_response);
	tcpSocket << response;
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

