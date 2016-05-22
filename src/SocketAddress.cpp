#include "SocketAddress.hpp"

void SocketAddress::init()
{
    std::memset(&this->internalAddress, 0, sizeof(struct sockaddr_in));
    this->internalAddress.sin_family = AF_INET;
}

SocketAddress::SocketAddress()
{
    this->init();
    this->internalAddress.sin_port = 0;
    this->internalAddress.sin_addr.s_addr = INADDR_ANY;
}

SocketAddress::SocketAddress(const std::string & association)
{
    this->init();
    this->setAssociation(association);
}

SocketAddress::SocketAddress(const char * association)
{
    this->init();
    this->setAssociation(association);
}

SocketAddress::~SocketAddress()
{
}

void SocketAddress::setPort(short port)
{
    this->internalAddress.sin_port = htons(port);
}

void SocketAddress::setPort(std::string port)
{
    short p = atoi(port.c_str());
    this->setPort(p);
}

int SocketAddress::setAddress(const std::string address)
{
    return inet_aton(address.c_str(), &this->internalAddress.sin_addr);
}

int SocketAddress::setAssociation(std::string association)
{
    auto position = association.find_first_of(":");
    auto address = association.substr(0, position);
    auto port = association.substr(position + 1, association.size());
    this->setPort(port);
    this->setAddress(address);
}

short SocketAddress::getPort()
{
    return ntohs(this->internalAddress.sin_port);
}

std::string SocketAddress::getAddress()
{
    return std::string(inet_ntoa(this->internalAddress.sin_addr));
}

struct sockaddr * SocketAddress::toInternalAddressStructPointer() const
{
    return (struct sockaddr *)(&(this->internalAddress));
}

unsigned int SocketAddress::getSize() const
{
    return sizeof(this->internalAddress);
}

SocketAddress::SocketAddress(int address, short port) {
    this->init();
    this->setAddress(address);
    this->setPort(port);
}

int SocketAddress::setAddress(int address) {
    this->internalAddress.sin_addr.s_addr = htonl(address);
}
