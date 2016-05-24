#ifndef DNS_CHECKER_HTTPSERVER_H
#define DNS_CHECKER_HTTPSERVER_H

#include "HTTPHandler.hpp"
#include "Configuration.hpp"
#include "TCPSocket.hpp"
#include <list>

class HTTPServer
{
public:
	HTTPServer();
	void listen();
private:
	const std::string invalid_request = "HTTP/1.1 418 I'm teapot\r\n"
					"Content-Length: 0\r\n"
					"Connection: Closed\r\n"
					"\r\n";

	const std::string valid_request = "HTTP/1.1 200 OK\r\n"
			"Content-Length: {0}\r\n"
			"Content-Type: application/json\r\n"
			"Connection: Closed\r\n";
	TCPSocket socket;

	void response(int clientSocket);
	std::string valid_request_function(std::string &response_json);
	std::string invalid_request_function();
};


#endif //DNS_CHECKER_HTTPSERVER_H
