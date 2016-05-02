#include "DNSPacket.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>

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


char *DNSQuestion::getRaw() {
    char *rawData = (char *) std::malloc(1 + this->qname.length() + 1 + 4);
    int i = 0;
    std::string domain = this->qname;
    size_t  pos;
    size_t len;
    do {
        pos = domain.find_first_of(".");
        std::string substr = domain.substr(0, pos);
        len = substr.length();
        rawData[i] = (char)len;
        std::memcpy(&rawData[i+1], substr.c_str(), len);
        domain.erase(0, len + 1);
        i += len+1;
    } while (pos != std::string::npos);
    rawData[i] = '\0';
    rawData[i+1] = '\0';
    rawData[i+2] = 1;
    rawData[i+3] = '\0';
    rawData[i+4] = 1;
    return rawData;
}

char *DNSQuestion::toPacketFormat(std::string domainName) {
    this->qname = domainName;
}


DNSQuestion::DNSQuestion(const std::string &qname) {
    this->qname = qname;
}