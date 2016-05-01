#include "DNSPacket.hpp"
#include <arpa/inet.h>

short DNSPacket::generateIdentifier() {
    std::srand(std::time(0));
    return std::rand() & 0xFFFF;
}

bool DNSPacket::parseRawBuffer(unsigned char *buffer, int size) {
    this->parsePointer = 0;
    this->identifier = (buffer[0] << 8) | buffer[1];
    short flags = (buffer[2] << 8) | buffer[3];
    this->response = flags & (1 << 15);
    this->answerIsAuthoritative = flags & (1 << 10);
    this->recursionDesired = flags & (1 << 8);
    this->recursionAvailable = flags & (1 << 7);
    this->responseCode = flags & 0xF;
    this->questionCount= buffer[4] << 8 | buffer[5];
    this->answerRecordCount = buffer[6] << 8 | buffer[7];
    this->authorityRecordCount= buffer[8] << 8 | buffer[9];
    this->additionalRecordCount = buffer[10] << 8 | buffer[11];
}

bool DNSPacket::isResponse() {
    return this->response;
}


char *DNSQuestion::getRawName() {

}

char *DNSQuestion::toPacketFormat(std::string domainName) {
}



