#include "UDPSocket.hpp"
#include "DNSPacket.hpp"
#include <cstdlib>

UDPSocket::UDPSocket() {
    this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

UDPSocket::UDPSocket(const SocketAddress &socketAddress) {
    this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
        int status = recvfrom(this->socketFileDescriptor,
                              (char *) data + recSize,
                              size, 0,
                              this->internalAddress.toInternalAddressStructPointer(),
                              &aSize);
        if (status > 0)
            return *this;
        else if (status < 0) {
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

UDPSocket &UDPSocket::operator<<(const DNSPacket &packet) {
    int size;
    char *data = packet.getRaw(&size);
    this->send(data, size);
    delete[] data;
    return *this;
}

UDPSocket &UDPSocket::operator>>(DNSPacket &packet) {
    this->setTimeout(1, 0);
    unsigned char *data = (unsigned char *) this->read(MAX_UDP_PACKET_SIZE);
    packet.parseRawBuffer(data, MAX_UDP_PACKET_SIZE);
    return *this;
}

char UDPSocket::readByte() {
    char byte;
    socklen_t aSize = this->internalAddress.getSize();
    ssize_t status = recvfrom(this->socketFileDescriptor, &byte,
                              1, 0,
                              this->internalAddress.toInternalAddressStructPointer(),
                              &aSize);
    if (status == 1) {
        return byte;
    }
    else if (status < 0) {
        TimeoutException e;
        throw e;
    }
}

void *UDPSocket::read(unsigned int size) {
    void *buffer = new char[size];
    socklen_t aSize = this->internalAddress.getSize();

    ssize_t sum_recived = 0;
    while (sum_recived < size) {
        ssize_t recived = recvfrom(this->socketFileDescriptor,
                                   (char *) buffer + sum_recived,
                                   size - sum_recived, 0,
                                   this->internalAddress.toInternalAddressStructPointer(),
                                   &aSize);

        if (recived > 0)
            sum_recived += recived;
        else if (recived < 0 && sum_recived == 0) {
            TimeoutException e;
            throw e;
        }
        else if (recived < 0 && sum_recived > 0) {
            break;
        }
    }
    return buffer;
}

UDPSocket &UDPSocket::operator>>(DNSQuestion &dnsQuestion) {
    dnsQuestion.qname.clear();
    int i = 0;
    while (true) {
        char byte = this->readByte();
        if (byte == 0) {
            dnsQuestion.qname.pop_back();
            break;
        }
        char block_size = byte;
        i++;
        for (int j = 0; j < block_size; j++) {
            dnsQuestion.qname += this->readByte();
        }
        dnsQuestion.qname += ".";
    }
    this->readByte();
    this->readByte();
    this->readByte();
    this->readByte();
    return *this;
}

UDPSocket &UDPSocket::operator>>(
        DNSAuthoritativeNameServer &authoritativeNameServer) {
    authoritativeNameServer.size = 0;
    if (this->readByte() & (0x3 << 6)) {
        authoritativeNameServer.size += 2;
        this->readByte();
    }
    //type
    this->readByte();
    this->readByte();
    authoritativeNameServer.size += 2;

    //class
    this->readByte();
    this->readByte();
    authoritativeNameServer.size += 2;

    //ttl
    this->readByte();
    this->readByte();
    this->readByte();
    this->readByte();
    authoritativeNameServer.size += 4;

    //string size;
    authoritativeNameServer.size += (this->readByte() << 8) | this->readByte();
    authoritativeNameServer.size += 2;

    return *this;
}

UDPSocket &UDPSocket::operator>>(DNSAdditionalRecord &additionalRecord) {
    additionalRecord.size = 0;
    if (this->readByte() & (0x3 << 6)) {
        additionalRecord.size += 2;
        this->readByte();
    }
    //type
    bool ipv4 = false;
    if (not (this->readByte() == 0x0 and this->readByte() == 0x1c))
        ipv4 = true;
    additionalRecord.size += 2;

    //class
    this->readByte();
    this->readByte();
    additionalRecord.size += 2;

    //ttl
    this->readByte();
    this->readByte();
    this->readByte();
    this->readByte();
    additionalRecord.size += 4;

    additionalRecord.size += (this->readByte() << 8) | this->readByte();
    additionalRecord.size += 2;

    if (ipv4) {
        additionalRecord.address =
                this->readByte() << 24 | this->readByte() << 16 |
                this->readByte() << 8 | this->readByte();
    }
    return *this;
}
