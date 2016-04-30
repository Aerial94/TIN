#include "Domain.h"

Domain::Domain(std::string dName)
{
    this->domainName = dName;
    this->status = DOMAIN_STATUS::FOLLOWED_BUT_NOT_CHECKED;
}


Domain::DOMAIN_STATUS Domain::getStatus()
{
    return this->status;
}


void Domain::setStatus(DOMAIN_STATUS status)
{
    this->status = status;
}


std::string Domain::getDomainName()
{
    return this->domainName;
}