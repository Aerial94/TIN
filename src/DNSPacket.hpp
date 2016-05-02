#ifndef DNS_CHECKER_DNSPACKET_HPP
#define DNS_CHECKER_DNSPACKET_HPP

#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

class DNSQuestion {
private:
    std::string qname;
    short qtype;
    short qclass;
public:
    DNSQuestion() {}
    DNSQuestion(const std::string &qname);
    char * toPacketFormat(std::string domainName);
    std::string fromPacketFormat(char *data, char* end);
    const std::string &getDomainName() const {
        return qname;
    }

    char *getRaw();
    int setRaw(char *raw);
};

class DNSAnswer {};

class DNSPacket {
private:
public:
    short getIdentifier() const {
        return identifier;
    }
    bool parseRawBuffer(unsigned char * buffer, int size);
    bool toRawBuffer(void * buffer, int * size);

    bool isResponse();

    bool isAnswerAuthoritative() const {
        return answerIsAuthoritative;
    }
    bool isRecursionDesired() const {
        return recursionDesired;
    }

    bool isRecursionAvailable() const {
        return recursionAvailable;
    }

    char getResponseCode() const {
        return  this->responseCode;
    }
    short getQuestionCount() const {
        return  this->questionCount;
    }
    short getAnswerCount() const {
        return  this->answerRecordCount;
    }
    short getAuthorityCount() const {
        return  this->authorityRecordCount;
    }

    short getAdditionalCount() const {
        return  this->additionalRecordCount;
    }

    short identifier;
    bool response;
    char opcode;
    bool answerIsAuthoritative;
    bool truncated;
    bool recursionDesired;
    bool recursionAvailable;
    char responseCode;
    short questionCount;
    short answerRecordCount;
    short authorityRecordCount;
    short additionalRecordCount;

    short generateIdentifier();
    int parsePointer;
    std::vector<DNSQuestion> questions;
};


#endif //DNS_CHECKER_DNSPACKET_HPP
