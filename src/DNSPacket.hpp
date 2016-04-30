#ifndef DNS_CHECKER_DNSPACKET_HPP
#define DNS_CHECKER_DNSPACKET_HPP

#include <cstdlib>
#include <ctime>

class DNSPacket {
private:
    short identifier;
    bool response;
    char opcode;
    bool answerIsAuthoritative;
    bool truncated;
    bool recursionDesired;
    bool recursionAvailable;
    bool responseCode;
    short questionCount;
    short answerRecordCount;
    short authorityRecordCount;
    short additionalRecordCount;
};


#endif //DNS_CHECKER_DNSPACKET_HPP
