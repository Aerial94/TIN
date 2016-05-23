#ifndef TCPSOCKET_HPP_
#define TCPSOCKET_HPP_
#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "Socket.hpp"
#include "SocketAddress.hpp"


class TCPSocket : public Socket
{
private:
    bool isConnected;

public:
    enum Exception {
        CONNECTION_FAILED,
        BIND_FAILED,
    };
    TCPSocket();
    TCPSocket(int socketFileDescriptor);
    virtual ~TCPSocket();
    int connect(SocketAddress & address);
    int bind(const SocketAddress & address);
    int listen(int backlog = 32);
    int accept();
    TCPSocket & operator<<(const std::string & data);
    std::string readLine();
	std::string read_from_socket(int size);
    void serveForever(int port);
    void bindAll(int port);
};

#endif /* TCPSOCKET_HPP_ */
