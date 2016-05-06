#ifndef DNS_CHECKER_HTTPSERVER_H
#define DNS_CHECKER_HTTPSERVER_H

#include "HTTPHandler.hpp"
#include "Configuration.hpp"
#include "TCPSocket.hpp"
#include <list>

class HTTPServer
{
public:
	void listen();
private:
	Configuration configuration;
	TCPSocket socket;
	HTTPHandler hander;
	void response(TCPSocket socekt);
	bool is_request_valid(std::string& request);
	std::string get_json(std::string& request);
	void valid_request(TCPSocket, std::string json);
	void invalid_request(TCPSocket);
};


#endif //DNS_CHECKER_HTTPSERVER_H
