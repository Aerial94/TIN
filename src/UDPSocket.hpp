#ifndef UDPSOCKET_HPP
#define UDPSOCKET_HPP
#include "Socket.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class DNSPacket;
class DNSQuestion;
class DNSAuthoritativeNameServer;
class DNSAdditionalRecord;

#define MAX_UDP_PACKET_SIZE 1 << 9

class UDPSocket : public Socket
{

  public:
    UDPSocket(const SocketAddress &socketAddress);
    UDPSocket();
    void setAddress(const SocketAddress &socketAddress);
    virtual ~UDPSocket();
    void bind(SocketAddress &socketAddress);
    char readByte();
    void *read(unsigned int size);
    UDPSocket &operator<<(const std::string &data);
    UDPSocket &operator<<(const DNSPacket &packet);
    UDPSocket &send(void *data, int size);
    UDPSocket &operator>>(std::string &data);
    UDPSocket &operator>>(DNSPacket &packet);
    UDPSocket &operator>>(DNSQuestion &dnsQuestion);
    UDPSocket &operator>>(DNSAuthoritativeNameServer &authoritativeNameServer);
    UDPSocket &operator>>(DNSAdditionalRecord &additionalRecord);
    UDPSocket &recive(void *data, int size);
};

#endif // UDPSOCKET_HPP
