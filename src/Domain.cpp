#include "Domain.hpp"

Domain::Domain(std::string dName)
{
    this->domainName = dName;
    this->status = DomainStatus::FOLLOWED_BUT_UNCHECKED;
    this->timestamp = 0;
}


Domain::DomainStatus Domain::getStatus()
{
    return this->status;
}


void Domain::setStatus(DomainStatus status)
{
    if (this->status != status) {
        /*
         * If status has changed then update timestamp
         */
        this->setTimestamp(time(0));
    }
    this->status = status;
}


std::string Domain::getDomainName()
{
    return this->domainName;
}

void Domain::setIP(std::string ipAddress) {
    this->ip = ipAddress;
}

std::string Domain::getIP() {
    return this->ip;
}

void Domain::setTimestamp(time_t timestamp) {
    this->timestamp = timestamp;
}


time_t Domain::getTimestamp() const {
    return this->timestamp;
}