#include "DNSPacket.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

short DNSPacket::generateIdentifier() const {
    std::srand(std::time(0));
    return std::rand() & 0xFFFF;
}

bool DNSPacket::parseRawBuffer(unsigned char *buffer, int size) {
    this->parsePointer = 0;
    this->identifier = (buffer[0] << 8) | buffer[1];
    short flags = (buffer[2] << 8) | buffer[3];
    this->response = (bool) (flags & (1 << 15));
    this->answerIsAuthoritative = (bool) (flags & (1 << 10));
    this->recursionDesired = (bool) (flags & (1 << 8));
    this->recursionAvailable = (bool) (flags & (1 << 7));
    this->responseCode = (char) (flags & 0xF);
    this->questionCount = (short) buffer[4] << 8 | buffer[5];
    this->answerRecordCount = buffer[6] << 8 | buffer[7];
    this->authorityRecordCount = buffer[8] << 8 | buffer[9];
    this->additionalRecordCount = buffer[10] << 8 | buffer[11];
    int i = 12;
    for (int j = 0; j < this->questionCount; j++) {
        DNSQuestion dnsQuestion;
        int size = dnsQuestion.setRaw((char *) &buffer[i]);
        this->questions.push_back(dnsQuestion);
        i += size;
    }
    for (int j = 0; j < this->answerRecordCount; j++) {
        DNSAnswer answer;
        bool ip = answer.setRaw((char *) &buffer[i]);
        this->answers.push_back(answer);
        i += answer.getSize();
    }
    for (int j = 0; j < this->authorityRecordCount; j++) {
        DNSAuthoritativeNameServer authoritativeNameServer;
        int k;
        authoritativeNameServer.fromRaw(&buffer[i], k, buffer);
        this->authority.push_back(authoritativeNameServer);
        i += authoritativeNameServer.getSize();
    }
    for (int j = 0; j < this->additionalRecordCount; j++) {
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

char *DNSPacket::getRaw(int *size) const {
    *size = 12;
    for (auto i = this->questions.begin(); i != this->questions.end(); i++) {
        *size += i->getSize();
    }
    char *header = new char[*size];
    std::memset(header, 0, *size);
    short id = this->generateIdentifier();
    header[0] = id >> 8 & 0xFF;
    header[1] = id & 0xFF;

    header[2] |= (this->response) << 0;
    header[2] |= (this->recursionDesired) << 7;

    header[4] = (this->questionCount >> 8) & 0xFF;
    header[5] = this->questionCount & 0xFF;

    int j = 12;
    for (auto i = this->questions.begin(); i != this->questions.end(); i++) {
        char *raw = i->getRaw();
        memcpy(&header[j], raw, i->getSize());
        j += i->getSize();
        free(raw);
    }
    return header;
}

void DNSPacket::markAsResponse() {
    this->response = true;
}

void DNSPacket::markAsQuestion() {
    this->response = false;
}


char *DNSQuestion::getRaw() const {
    char *rawData = (char *) std::malloc(1 + this->qname.length() + 1 + 4);
    int i = 0;
    std::string domain = this->qname;
    size_t pos;
    size_t len;
    do {
        pos = domain.find_first_of(".");
        std::string substr = domain.substr(0, pos);
        len = substr.length();
        rawData[i] = (char) len;
        std::memcpy(&rawData[i + 1], substr.c_str(), len);
        domain.erase(0, len + 1);
        i += len + 1;
    } while (pos != std::string::npos);
    rawData[i] = '\0';
    rawData[i + 1] = '\0';
    rawData[i + 2] = 1;
    rawData[i + 3] = '\0';
    rawData[i + 4] = 1;
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
    while (raw[i] != 0) {
        size_t block_size = raw[i];
        i++;
        for (int j = 0; j < block_size; j++) {
            this->qname += raw[i++];
        }
        if (raw[i + 1]) {
            this->qname += ".";
        }
    }
    return i + 1 + 4;
}

int DNSQuestion::getSize() const {
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

std::vector<DNSAuthoritativeNameServer> DNSPacket::getAuthorityNameServers() {
    return this->authority;
}


bool DNSAuthoritativeNameServer::fromRaw(unsigned char *data, int len,
                                         unsigned char *allData) {
    this->size = 0;
    if (data[0] & (0x3 << 6)) {
        size += 2;
    }
    size += 4;
    size += 4;
    size += 2;
    int i = size;
    size += (data[10] << 8) | data[11];
    this->qname.clear();
    while (data[i] != 0) {
        size_t block_size = data[i];
        if ((block_size & (3 << 6)) == (3 << 6)) {
            int offset = (int) (((block_size & (~(3 << 6))) << 8) | data[++i]);
            while (allData[offset] != 0) {
                size_t block_size = allData[offset++];
                if ((block_size & (3 << 6)) == (3 << 6)) {
                    offset = (int) (((block_size & (~(3 << 6))) << 8) |
                                    allData[offset]);
                    continue;
                }
                for (int j = 0; j < block_size; j++) {
                    this->qname += allData[offset++];
                }
                if (allData[i + 1]) {
                    this->qname += ".";
                }
            }
            break;
        }
        i++;
        for (int j = 0; j < block_size; j++) {
            this->qname += data[i++];
        }
        if (data[i + 1]) {
            this->qname += ".";
        }
    }
    if (this->qname[this->qname.length() - 1] == '.') {
        this->qname.pop_back();
    }
    return true;
}

int DNSAuthoritativeNameServer::getSize() {
    return this->size;
}

std::string DNSAuthoritativeNameServer::getQName() {
    return this->qname;
}


bool DNSAdditionalRecord::fromRaw(unsigned char *data, int len) {
    this->size = 0;
    if ((data [0] & (3 << 6)) == (3 << 6)) {
        size += 2;
    }
    size += 4;
    size += 4;
    size += 2;
    this->address = *(int *) &data[size];
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
    size_t pos;
    size_t len;
    do {
        pos = domain.find_first_of(".");
        std::string substr = domain.substr(0, pos);
        len = substr.length();
        rawData[i] = (char) len;
        std::memcpy(&rawData[i + 1], substr.c_str(), len);
        domain.erase(0, len + 1);
        i += len + 1;
    } while (pos != std::string::npos);
    rawData[i] = '\0';
    return rawData;
}

void FQDN::fromRaw(unsigned char *raw) {
    this->name.clear();
    int i = 0;
    while (raw[i] != 0) {
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
        if (raw[i + 1]) {
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

bool DNSAnswer::setRaw(char *data) {
    this->size = 0;
    if ((data [0] & (3 << 6)) == (3 << 6)) {
        size += 2;
    }
    size += 4;
    size += 4;
    size += 2;
    this->address = *(int *) &data[size];
    size += (data[10] << 8) | data[11];
    if (data[2] == 0x0 and data[3] == 0x1c) {
        return false;
    }
    return true;
}

int DNSAnswer::getSize() {
    return this->size;
}


std::string hostnameToIP(std::string hostname) {
    {
        struct hostent *he;
        struct in_addr **addr_list;
        int i;

        if ( (he = gethostbyname( hostname.c_str()) ) == NULL)
        {
            TimeoutException exception;
            throw exception;
        }

        addr_list = (struct in_addr **) he->h_addr_list;

        for(i = 0; addr_list[i] != NULL; i++)
        {
            char * ip = inet_ntoa(*addr_list[i]);
            return std::string(ip);
        }
    }
}