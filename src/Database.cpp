#include "Database.hpp"

std::vector<Domain> Database::domains;
std::mutex Database::mutex;


std::vector<Domain>::iterator Database::findDomain(std::string dName)
{
    std::lock_guard<std::mutex> guard(mutex);
    for(std::vector<Domain>::iterator it = domains.begin(); it != domains.end(); ++it)
    {
        if((*it).getDomainName() == dName)
            return it;
    }

    return domains.end();
}


HTTPHandler::MessageStatus Database::getDomainStatus(std::string dName)
{
    std::lock_guard<std::mutex> guard(mutex);
    if(findDomain(dName) != domains.end() && !domains.empty())
    {
        switch((*findDomain(dName)).getStatus())
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
    if(findDomain(dName) != domains.end() && !domains.empty())
        return HTTPHandler::MessageStatus::ALREADY_IN_DATABASE;

    domains.push_back(Domain(dName));
    return HTTPHandler::MessageStatus::OK;
}


HTTPHandler::MessageStatus Database::removeDomain(std::string dName)
{
    std::lock_guard<std::mutex> guard(mutex);
    if(findDomain(dName) != domains.end())
    {
        domains.erase(findDomain(dName));
        return HTTPHandler::MessageStatus::OK;
    }
    return HTTPHandler::MessageStatus::NO_IN_DATABASE;
}


HTTPHandler::MessageStatus Database::updateDomain(std::string dName, Domain::DomainStatus status)
{
    std::lock_guard<std::mutex> guard(mutex);
    if(findDomain(dName) != domains.end())
    {
        (*findDomain(dName)).setStatus(status);
        return HTTPHandler::MessageStatus::OK;
    }
    return HTTPHandler::MessageStatus::NO_IN_DATABASE;
}