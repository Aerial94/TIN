#ifndef DNS_CHECKER_DATABASE_H
#define DNS_CHECKER_DATABASE_H

#include <vector>
#include <string>
#include <iterator>
#include <mutex>
#include "Domain.hpp"
#include "HTTPHandler.hpp"

class Database
{
private:
    Database() {}
    Database(const Database &);
    ~Database() {}
    Database & operator=(const Database &);
    std::vector<Domain>::iterator findDomain(std::string dName);

    std::vector<Domain> domains;
    std::mutex mutex;
    std::vector<Domain>::iterator currentPos;

public:
    static Database & getInstance()
    {
        static Database instance;
        return instance;
    }

    HTTPHandler::MessageStatus getDomainStatus(std::string dName);
    HTTPHandler::MessageStatus addDomain(std::string dName);
    HTTPHandler::MessageStatus removeDomain(std::string dName);
    HTTPHandler::MessageStatus updateDomain(std::string dName, Domain::DomainStatus status);
    std::vector<std::string> copy();
    std::string getNextDomain();

    std::vector<Domain>::iterator unsafeFindDomain(std::string domainName);
    void clear();
};


#endif //DNS_CHECKER_DATABASE_H
