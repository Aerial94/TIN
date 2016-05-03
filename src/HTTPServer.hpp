#ifndef DNS_CHECKER_HTTPSERVER_H
#define DNS_CHECKER_HTTPSERVER_H

#include <thread>
#include "HTTPHander.hpp"
#include <list>

class HTTPServer
{
public:
	void listen();
private:
	TCPSocket socket;
	void wait_for_reuqest();

};


#endif //DNS_CHECKER_HTTPSERVER_H
