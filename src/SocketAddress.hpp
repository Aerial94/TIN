#ifndef SOCKETADDRESS_HPP_
#define SOCKETADDRESS_HPP_
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

class SocketAddress
{
private:
    struct sockaddr_in internalAddress;
    void init();

public:
    SocketAddress();
    SocketAddress(const std::string & association);
    SocketAddress(const char * association);
    SocketAddress(int address, short port);
    virtual ~SocketAddress();

    void setPort(short port);
    void setPort(std::string port);
    int setAddress(const std::string address);
    int setAddress(int address);
    int setAssociation(std::string association);
    short getPort();
    std::string getAddress();
    struct sockaddr * toInternalAddressStructPointer() const;
    unsigned int getSize() const;
};

#endif /* SOCKETADDRESS_HPP_ */
