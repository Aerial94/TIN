#include "Socket.hpp"

Socket::Socket() {
    this->timeout.tv_sec = 0;
    this->timeout.tv_usec = 0;
}

Socket::~Socket() {
}

void Socket::setTimeout(long int sec, long int usec) {
    this->timeout.tv_sec = sec;
    this->timeout.tv_usec = usec;
    setsockopt(this->socketFileDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (void *)(&(this->timeout)), sizeof(struct timeval));
}

void Socket::setReuseAddress() {
    int enable = 1;
    setsockopt(this->socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &enable,
               sizeof(int));
}
