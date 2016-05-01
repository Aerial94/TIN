#ifndef DNS_CHECKER_DATABASE_H
#define DNS_CHECKER_DATABASE_H

#include "Domain.hpp"
#include "HTTPHandler.hpp"
#include <vector>
#include <string>
#include <iterator>
#include <mutex>

class Database
{
private:
    Database() {};
    //~Database() {};
    static std::vector<Domain>::iterator findDomain(std::string dName);

    static std::vector<Domain> domains;
    static std::mutex mutex;

public:
    static Database & getInstance()
    {
        static Database instance;
        return instance;
    }


    static HTTPHandler::MessageStatus getDomainStatus(std::string dName);
    static HTTPHandler::MessageStatus addDomain(std::string dName);
    static HTTPHandler::MessageStatus removeDomain(std::string dName);
    static HTTPHandler::MessageStatus updateDomain(std::string dName, Domain::DomainStatus status);
};


#endif //DNS_CHECKER_DATABASE_H
