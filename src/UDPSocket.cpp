#include "UDPSocket.hpp"

UDPSocket::UDPSocket() {
    this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
}

UDPSocket::UDPSocket(const SocketAddress &socketAddress) {
    this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    this->setAddress(socketAddress);
}

UDPSocket::~UDPSocket() {
    ::close(this->socketFileDescriptor);
}

void UDPSocket::setAddress(const SocketAddress &socketAddress) {
    this->internalAddress = socketAddress;
}

void UDPSocket::bind(SocketAddress &socketAddress) {
    ::bind(this->socketFileDescriptor,
           socketAddress.toInternalAddressStructPointer(),
           socketAddress.getSize());
}

UDPSocket &UDPSocket::operator<<(const std::string &data) {
    sendto(this->socketFileDescriptor, data.c_str(), data.length(), 0,
           this->internalAddress.toInternalAddressStructPointer(),
           this->internalAddress.getSize());
    return *this;
}

UDPSocket &UDPSocket::operator>>(std::string &data) {
    char buffer[MAX_UDP_PACKET_SIZE];
    socklen_t size = this->internalAddress.getSize();
    ssize_t recSize;
    recSize = recvfrom(this->socketFileDescriptor, buffer,
                       MAX_UDP_PACKET_SIZE, 0,
                       this->internalAddress.toInternalAddressStructPointer(),
                       &size);
    buffer[recSize] = '\0';
    data.append(buffer);
    return *this;
}

UDPSocket &UDPSocket::recive(void *data, int size) {
    socklen_t aSize = this->internalAddress.getSize();
    int recSize = recvfrom(this->socketFileDescriptor, data,
                       size, 0,
                       this->internalAddress.toInternalAddressStructPointer(),
                       &aSize);
    return *this;
}

UDPSocket &UDPSocket::send(void *data, int size) {
    int status = sendto(this->socketFileDescriptor, data, size, 0,
           this->internalAddress.toInternalAddressStructPointer(),
           this->internalAddress.getSize());
    return *this;
}













