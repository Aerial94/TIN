#ifndef UDPSOCKET_HPP
#define UDPSOCKET_HPP
#include "Socket.hpp"

#include <sys/types.h>
#include <sys/socket.h>


#define MAX_UDP_PACKET_SIZE 1<<9

class UDPSocket : public  Socket {

public:
    UDPSocket(const SocketAddress &socketAddress);

    virtual ~UDPSocket();
    void bind(SocketAddress & socketAddress);
    UDPSocket &operator<<(const std::string &data);
    UDPSocket &operator>>(std::string &data);
};


#endif //UDPSOCKET_HPP
