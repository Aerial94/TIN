#include "Database.hpp"

std::vector<Domain>::iterator Database::findDomain(std::string dName)
{
    std::lock_guard<std::mutex> guard(mutex);
    return this->unsafeFindDomain(dName);
}


HTTPHandler::MessageStatus Database::getDomainStatus(std::string dName)
{
    std::lock_guard<std::mutex> guard(mutex);
    if(unsafeFindDomain(dName) != domains.end() && !domains.empty())
    {
        switch((*unsafeFindDomain(dName)).getStatus())
        {
            case Domain::DomainStatus::FOLLOWED:
                return HTTPHandler::MessageStatus::OK;
            case Domain::DomainStatus ::FOLLOWED_BUT_UNCHECKED:
                return HTTPHandler::MessageStatus::UNKNOWN;
            case Domain::DomainStatus::NONEXISTENT:
                return HTTPHandler::MessageStatus::UNREACHABLE;
        }
    }
    return HTTPHandler::NO_IN_DATABASE;
}


HTTPHandler::MessageStatus Database::addDomain(std::string dName)
{
    std::lock_guard<std::mutex> guard(mutex);
    //check if it does't already exist in database
    if(unsafeFindDomain(dName) != domains.end() && !domains.empty())
        return HTTPHandler::MessageStatus::ALREADY_IN_DATABASE;

    domains.push_back(Domain(dName));
    return HTTPHandler::MessageStatus::OK;
}


HTTPHandler::MessageStatus Database::removeDomain(std::string dName)
{
    std::lock_guard<std::mutex> guard(mutex);
    if(unsafeFindDomain(dName) != domains.end())
    {
        domains.erase(unsafeFindDomain(dName));
        return HTTPHandler::MessageStatus::OK;
    }
    return HTTPHandler::MessageStatus::NO_IN_DATABASE;
}


HTTPHandler::MessageStatus Database::updateDomain(std::string dName, Domain::DomainStatus status)
{
    std::lock_guard<std::mutex> guard(mutex);
    if(unsafeFindDomain(dName) != domains.end())
    {
        (*unsafeFindDomain(dName)).setStatus(status);
        return HTTPHandler::MessageStatus::OK;
    }
    return HTTPHandler::MessageStatus::NO_IN_DATABASE;
}

std::string Database::getNextDomain()
{
    std::lock_guard<std::mutex> guard(mutex);
    if(!this->domains.empty() && this->currentPos != this->domains.end())
    {
        if (this->currentPos.base() == nullptr) {
            this->currentPos = this->domains.begin();
        }
        std::string toReturn = this->currentPos->getDomainName();
        ++(this->currentPos);
        return toReturn;
    }
    else
    {
        this->currentPos = this->domains.begin();
        return "";
    }
}

std::vector<Domain>::iterator Database::unsafeFindDomain(std::string domainName) {
    for(std::vector<Domain>::iterator it = domains.begin(); it != domains.end(); ++it)
    {
        if((*it).getDomainName() == domainName)
            return it;
    }
    return domains.end();
}

void Database::clear() {
    this->domains.clear();
}



