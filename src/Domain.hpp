#ifndef DNS_CHECKER_DOMAIN_H
#define DNS_CHECKER_DOMAIN_H
#include <string>

class Domain
{
public:
    enum DomainStatus {FOLLOWED, FOLLOWED_BUT_UNCHECKED, NONEXISTENT};
private:
    std::string domainName;
    DomainStatus status;

public:
    Domain(std::string dName);
    DomainStatus getStatus();
    void setStatus(DomainStatus status);
    std::string getDomainName();
};


#endif //DNS_CHECKER_DOMAIN_H
