#include "UDPSocket.hpp"

UDPSocket::UDPSocket() {
    this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(this->socketFileDescriptor, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
}

UDPSocket::UDPSocket(const SocketAddress &socketAddress) {
    this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(this->socketFileDescriptor, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
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
    int recSize = 0;
    while (recSize < size) {
        int status = recvfrom(this->socketFileDescriptor, (char*)data+recSize,
                            size, 0,
                            this->internalAddress.toInternalAddressStructPointer(),
                            &aSize);
        if (status > 0)
            return *this;
        else if (status < 0){
            TimeoutException e;
            throw e;
        }
    }
    return *this;
}

UDPSocket &UDPSocket::send(void *data, int size) {
    ssize_t status = sendto(this->socketFileDescriptor, data, size, 0,
           this->internalAddress.toInternalAddressStructPointer(),
           this->internalAddress.getSize());
    return *this;
}













