#include "Domain.hpp"

Domain::Domain(std::string dName)
{
    this->domainName = dName;
    this->status = DomainStatus::FOLLOWED_BUT_UNCHECKED;
}


Domain::DomainStatus Domain::getStatus()
{
    return this->status;
}


void Domain::setStatus(DomainStatus status)
{
    this->status = status;
}


std::string Domain::getDomainName()
{
    return this->domainName;
}