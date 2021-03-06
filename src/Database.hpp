#ifndef DNS_CHECKER_DATABASE_H
#define DNS_CHECKER_DATABASE_H

#include "Domain.hpp"
#include "HTTPHandler.hpp"
#include <iterator>
#include <mutex>
#include <string>
#include <vector>

class Database
{
  private:
    Database() {
    }
    Database(const Database &);
    ~Database() {
    }
    Database &operator=(const Database &);
    std::vector<Domain>::iterator findDomain(std::string dName);

    std::vector<Domain> domains;
    std::mutex mutex;
    std::vector<Domain>::iterator currentPos;

  public:
    static Database &getInstance() {
        static Database instance;
        return instance;
    }

    std::pair<HTTPHandler::MessageStatus, Domain>
    getDomainStatus(std::string dName);
    HTTPHandler::MessageStatus addDomain(std::string dName);
    HTTPHandler::MessageStatus removeDomain(std::string dName);
    HTTPHandler::MessageStatus updateDomain(std::string dName,
                                            Domain::DomainStatus status);
    HTTPHandler::MessageStatus updateDomain(std::string dName,
                                            std::string ipAddress);
    std::vector<Domain> copy();
    std::string getNextDomain();

    std::vector<Domain>::iterator unsafeFindDomain(std::string domainName);
    void clear();
};

#endif // DNS_CHECKER_DATABASE_H
