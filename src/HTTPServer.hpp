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
	const std::string invalid_request = "HTTP/1.1 418 I'm teapot\n"
					"Content-Length: 230\n"
					"Content-Type: text/html; charset=iso-8859-1\n"
					"Connection: Closed\n";

	const std::string invalid_request = "HTTP/1.1 200 OK\n"
			"Content-Length: 88\n"
			"Content-Type: application/json\n"
			"Connection: Closed\n"
	Configuration configuration;
	TCPSocket socket;
	HTTPHandler hander;
	void response(TCPSocket& socekt);
	bool is_request_valid(std::string& request);
	std::string get_json(std::string& request);
	void valid_request(std::string& json);
	void invalid_request();
};


#endif //DNS_CHECKER_HTTPSERVER_H
