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
    this->questionCount= (short)buffer[4] << 8 | buffer[5];
    this->answerRecordCount = buffer[6] << 8 | buffer[7];
    this->authorityRecordCount= buffer[8] << 8 | buffer[9];
    this->additionalRecordCount = buffer[10] << 8 | buffer[11];
    int i = 12;
    for (int j = 0;j < this->questionCount; j++){
        DNSQuestion dnsQuestion;
        int size = dnsQuestion.setRaw((char *) &buffer[i]);
        this->questions.push_back(dnsQuestion);
        i+=size;
    }
    for (int j = 0;j < this->authorityRecordCount; j++){
        DNSAuthoritativeNameServer authoritativeNameServer;
        int k;
        authoritativeNameServer.fromRaw(&buffer[i], k);
        i+=authoritativeNameServer.getSize();
    }
    for (int j = 0;j < this->additionalRecordCount; j++){
        DNSAdditionalRecord additionalRecord;
        int k;
        bool ip = additionalRecord.fromRaw(&buffer[i], i);
        if (ip) {
            this->additional.push_back(additionalRecord);
        }
        i += additionalRecord.getSize();
    }
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

bool DNSPacket::isOk() {
    return not this->responseCode;
}

std::vector<DNSAdditionalRecord> &DNSPacket::getAdditional() {
    return this->additional;
}


bool DNSAuthoritativeNameServer::fromRaw(unsigned char *data, int len) {
    this->size = 0;
    if (data[0] & (0x3 << 6)) {
        size += 2;
    }
    size += 4;
    size += 4;
    size += 2;
    size += (data[10] << 8) | data[11];
    return true;
}

int DNSAuthoritativeNameServer::getSize() {
    return this->size;
}


bool DNSAdditionalRecord::fromRaw(unsigned char *data, int len) {
    this->size = 0;
    if (data[0] & (0x3 << 6)) {
        size += 2;
    }
    size += 4;
    size += 4;
    size += 2;
    this->address = *(int*)&data[size];
    size += (data[10] << 8) | data[11];
    if (data[2] == 0x0 and data[3] == 0x1c) {
        return false;
    }
    return true;
}

int DNSAdditionalRecord::getSize() {
    return this->size;
}

in_addr_t DNSAdditionalRecord::getIP() {
    return this->address;
}

char *FQDN::toRaw() {
    char *rawData = (char *) std::malloc(1 + this->name.length());
    int i = 0;
    std::string domain = this->name;
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
    return rawData;
}

void FQDN::fromRaw(unsigned char *raw) {
    this->name.clear();
    int i = 0;
    while(raw[i] != 0) {
        if (raw[i] & 3 << 6) {
            //pointer case
            this->pointer = raw[i + 1];
            break;
        }
        char block_size = raw[i];
        i++;
        for (int j = 0; j < block_size; j++) {
            this->name += raw[i++];
        }
        if (raw[i + 1]){
            this->name += ".";
        }
    }

}

std::string FQDN::getName() {
    return this->name;
}

bool FQDN::havePointer() {
    return this->pointer != 0;
}

short FQDN::getPointer() {
    return this->pointer;
}
