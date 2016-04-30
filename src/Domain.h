#ifndef DNS_CHECKER_DOMAIN_H
#define DNS_CHECKER_DOMAIN_H
#include <string>

class Domain
{
public:
    enum DOMAIN_STATUS {FOLLOWED, FOLLOWED_BUT_NOT_CHECKED, NONEXISTENT};
private:
    std::string domainName;
    DOMAIN_STATUS status;

public:
    Domain(std::string dName);
    DOMAIN_STATUS getStatus();
    void setStatus(DOMAIN_STATUS status);
    std::string getDomainName();
};


#endif //DNS_CHECKER_DOMAIN_H
