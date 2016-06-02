#include "TCPSocket.hpp"

/**
 * Default constructor for TCPSocket
 */
TCPSocket::TCPSocket() {
    this->bufferNotEmpty = false;
    this->currentReadPos = 0;
    this->isConnected = 0;
    this->socketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TCPSocket::TCPSocket(int socketFileDescriptor) {
    this->socketFileDescriptor = socketFileDescriptor;
    this->bufferNotEmpty = false;
    this->currentReadPos = 0;
    this->isConnected = 0;
}

TCPSocket::~TCPSocket() {
    close(this->socketFileDescriptor);
}

int TCPSocket::connect(SocketAddress &address) {
    int status =
            ::connect(this->socketFileDescriptor,
                      address.toInternalAddressStructPointer(),
                      address.getSize());
    return status;
}

int TCPSocket::bind(const SocketAddress &address) {
    int result =
            ::bind(this->socketFileDescriptor,
                   address.toInternalAddressStructPointer(), address.getSize());
    return result;
}

int TCPSocket::listen(int backlog) {
    ::listen(this->socketFileDescriptor, backlog);
}

int TCPSocket::accept() {
    fd_set rfds;
    struct timeval timeout = this->timeout;
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(this->socketFileDescriptor, &rfds);
        int retval = select(this->socketFileDescriptor + 1, &rfds, nullptr,
                            nullptr,
                            &timeout);
        timeout = this->timeout;
        if (retval > 0) {
            return ::accept(this->socketFileDescriptor, 0, 0);
        }
        else {
            /*Timeout or error*/
            TimeoutException e;
            throw e;
        }
    }
}

TCPSocket &TCPSocket::operator<<(const std::string &data) {
    send(this->socketFileDescriptor, data.c_str(), data.size(), 0);
    return *this;
}

std::string TCPSocket::readLine() {
    std::string data = "";
    char buffer;
    while (1) {
        buffer = this->readByte();
        if (buffer == EOF) {
            break;
        }
        data += buffer;
        if (buffer == '\n')
            break;
    }
    if (data == "") {
        TimeoutException e;
        throw e;
    }
    return data;
}

std::string TCPSocket::read_from_socket(unsigned int size) {
    std::string data;
    while (size--) {
        char c = this->readByte();
        if (c == EOF) {
            break;
        }
        data += c;
    }
    if (data == "") {
        TimeoutException e;
        throw e;
    }
    return data;
}


void TCPSocket::serveForever(int port) {
    this->bindAll(port);
    this->listen();
}

void TCPSocket::bindAll(int port) {
    SocketAddress address;
    address.setPort(port);
    this->bind(address);
}

/*
 * Read from socket with bufferd IO
 */
char TCPSocket::readByte() {
    if (this->bufferNotEmpty) {
        if (this->currentReadPos < this->bufferSize) {
            return this->buffer[this->currentReadPos++];
        }
        this->bufferNotEmpty = false;
    }
    ssize_t receive_size = 0;
    while (1) {
        receive_size = recv(this->socketFileDescriptor, &(this->buffer), 4096,
                           0);
        if (receive_size > 0) {
            this->bufferNotEmpty = true;
            this->currentReadPos = 0;
            this->bufferSize = (int) receive_size;
            return this->readByte();
        }
        else if (receive_size <= 0) {
            this->bufferNotEmpty = false;
            return EOF;
        }
    }
}
