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

void DNSPacket::addQuestion(DNSQuestion dnsQuestion) {
    this->questionCount++;
    this->questions.push_back(dnsQuestion);
}

char *DNSPacket::getRaw(int *size) {
    *size = 6 * sizeof(short);
    for (auto i = this->questions.begin(); i != this->questions.end(); i++) {
        *size += i->getSize();
    }
    char * header = (char *) malloc(*size);
    short id = this->generateIdentifier();
    header[0] = id >> 8 & 0xFF;
    header[1] = id & 0xFF;

    header[2] &= 0;
    header[2] |= (this->response) << 0;
    header[2] |= (this->recursionDesired) << 7;

    header[3] &= 0;
    header[4] = (this->questionCount >> 8) & 0xFF;
    header[5] = this->questionCount & 0xFF;

    header[6] = 0;
    header[7] = 0;
    header[8] = 0;
    header[9] = 0;
    header[10] = 0;
    header[11] = 0;
    int j = 12;
    for (auto i = this->questions.begin(); i != this->questions.end(); i++) {
        char * raw = i->getRaw();
        memcpy(&header[j],raw, i->getSize());
        j += i->getSize();
    }
    return header;
}

void DNSPacket::markAsResponse() {
    this->response = true;
}

void DNSPacket::markAsQuestion() {
    this->response = false;
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

int DNSQuestion::setRaw(char *raw) {
    this->qname.clear();
    int i = 0;
    while(raw[i] != 0) {
        size_t block_size = raw[i];
        i++;
        for (int j = 0; j < block_size; j++) {
            this->qname += raw[i++];
        }
        if (raw[i + 1]){
            this->qname += ".";
        }
    }
    return i+1+4;
}

int DNSQuestion::getSize() {
    return 1 + this->qname.length() + 1 + 4;
}

DNSPacket::DNSPacket() {
    this->questionCount = 0;
    this->answerRecordCount = 0;
    this->authorityRecordCount = 0;
    this->additionalRecordCount = 0;
}

void DNSAuthoritativeNameServer::fromRaw(unsigned char *data, int len) {
    this->size = 0;
    if (data[0] & (0x3 << 6)) {
        size += 2;
    }
    size += 4;
    size += 4;
    size += 2;
    size += (data[10] << 8) | data[11];
}

int DNSAuthoritativeNameServer::getSize() {
    return this->size;
}

