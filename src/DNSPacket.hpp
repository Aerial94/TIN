#ifndef DNS_CHECKER_DNSPACKET_HPP
#define DNS_CHECKER_DNSPACKET_HPP

#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

class FQDN {
    std::string name;
private:
    short pointer;
public:
    FQDN() {
        this->pointer = 0;
    }
    char * toRaw();
    void fromRaw(unsigned char *name);
    std::string getName();
    bool havePointer();
    short getPointer();

};

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
    int getSize();
};

class DNSAnswer {

};

class DNSAuthoritativeNameServer {
    std::string name;
    short type;
    unsigned int timeToLive;
    short dataLenght;
    std::string nameServer;
    int size;
public:
    bool fromRaw(unsigned char *data, int len);

    int getSize();
};

class DNSAdditionalRecord {
    std::string name;
    short type;
    short classType;
    int timeToLive;
    short dataLenght;
    int address;
    int size;
public:
    bool fromRaw(unsigned char *data, int len);
    int getSize();

    unsigned int getIP();
};

class DNSPacket {
private:
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
    std::vector<DNSAnswer> answers;
    std::vector<DNSAdditionalRecord> additional;
public:
    DNSPacket();
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
        return this->responseCode;
    }
    short getQuestionCount() const {
        return  this->questionCount;
    }
    short getAnswerCount() const {
        return this->answerRecordCount;
    }
    short getAuthorityCount() const {
        return this->authorityRecordCount;
    }

    short getAdditionalCount() const {
        return this->additionalRecordCount;
    }
    void addQuestion(DNSQuestion dnsQuestion);
    char *getRaw(int *size);
    void markAsResponse();
    void markAsQuestion();
    bool isOk();
    std::vector<DNSAdditionalRecord>& getAdditional();

};



#endif //DNS_CHECKER_DNSPACKET_HPP
