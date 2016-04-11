#ifndef SOCKET_HPP_
#define SOCKET_HPP_
#include "SocketAddress.hpp"
#include <sys/socket.h>
#include <sys/time.h>


class Socket
{
protected:
    int socketFileDescriptor;
    SocketAddress internalAddress;
    struct timeval timeout;

public:
    Socket();
    virtual ~Socket();
    void setTimeout(long int sec, long int usec);
    void setReuseAddress();
};

#endif /* SOCKET_HPP_ */
