#include "Database.hpp"
#include "Logger.hpp"

std::vector<Domain>::iterator Database::findDomain(std::string dName) {
    std::lock_guard<std::mutex> guard(mutex);
    return this->unsafeFindDomain(dName);
}

std::pair<HTTPHandler::MessageStatus, Domain>
Database::getDomainStatus(std::string dName) {
    Logger::getInstance().logInfo("Database", "Query domain: " + dName);
    std::lock_guard<std::mutex> guard(mutex);
    std::vector<Domain>::iterator domain;
    if ((domain = unsafeFindDomain(dName)) != domains.end() &&
        !domains.empty()) {
        HTTPHandler::MessageStatus status;
        switch (domain->getStatus()) {
        case Domain::DomainStatus::FOLLOWED:
            status = HTTPHandler::MessageStatus::OK;
            ;
        case Domain::DomainStatus::FOLLOWED_BUT_UNCHECKED:
            status = HTTPHandler::MessageStatus::UNKNOWN;
        case Domain::DomainStatus::NONEXISTENT:
            status = HTTPHandler::MessageStatus::UNREACHABLE;
        }
        std::pair<HTTPHandler::MessageStatus, Domain> p =
            std::make_pair(status, *domain);
        return p;
    }
    std::pair<HTTPHandler::MessageStatus, Domain> p = std::make_pair(
        HTTPHandler::MessageStatus::NO_IN_DATABASE, Domain("dummy"));
    return p;
}

HTTPHandler::MessageStatus Database::addDomain(std::string dName) {
    Logger::getInstance().logInfo("Database", "Saving domain: " + dName);
    std::lock_guard<std::mutex> guard(mutex);
    // check if it does't already exist in database
    if (unsafeFindDomain(dName) != domains.end() && !domains.empty())
        return HTTPHandler::MessageStatus::ALREADY_IN_DATABASE;
    Domain domain(dName);

    /*
     * Set timestamp for first time when domain is saved in database
     */
    domain.setTimestamp(time(0));

    domains.push_back(domain);
    return HTTPHandler::MessageStatus::OK;
}

HTTPHandler::MessageStatus Database::removeDomain(std::string dName) {
    Logger::getInstance().logInfo("Database", "Remove domain: " + dName);
    std::lock_guard<std::mutex> guard(mutex);
    std::vector<Domain>::iterator domain;
    if ((domain = unsafeFindDomain(dName)) != domains.end()) {
        domains.erase(domain);
        return HTTPHandler::MessageStatus::OK;
    }
    return HTTPHandler::MessageStatus::NO_IN_DATABASE;
}

HTTPHandler::MessageStatus Database::updateDomain(std::string dName,
                                                  Domain::DomainStatus status) {
    std::lock_guard<std::mutex> guard(mutex);
    std::vector<Domain>::iterator domain;
    if ((domain = unsafeFindDomain(dName)) != domains.end()) {
        domain->setStatus(status);
        return HTTPHandler::MessageStatus::OK;
    }
    return HTTPHandler::MessageStatus::NO_IN_DATABASE;
}

std::string Database::getNextDomain() {
    std::lock_guard<std::mutex> guard(mutex);
    if (!this->domains.empty() && this->currentPos != this->domains.end()) {
        if (this->currentPos.base() == nullptr) {
            this->currentPos = this->domains.begin();
        }
        std::string toReturn = this->currentPos->getDomainName();
        ++(this->currentPos);
        return toReturn;
    }
    else {
        this->currentPos = this->domains.begin();
        return "";
    }
}

std::vector<Domain>::iterator
Database::unsafeFindDomain(std::string domainName) {
    for (std::vector<Domain>::iterator it = domains.begin();
         it != domains.end(); ++it) {
        if ((*it).getDomainName() == domainName)
            return it;
    }
    return domains.end();
}

void Database::clear() {
    this->domains.clear();
}

std::vector<Domain> Database::copy() {
    std::lock_guard<std::mutex> guard(mutex);
    std::vector<Domain> d;
    for (Domain &dom : this->domains) {
        d.push_back(dom);
    }
    return d;
}

HTTPHandler::MessageStatus Database::updateDomain(std::string dName,
                                                  std::string ipAddress) {
    std::lock_guard<std::mutex> guard(mutex);
    std::vector<Domain>::iterator domain;
    if ((domain = unsafeFindDomain(dName)) != domains.end()) {
        domain->setStatus(Domain::DomainStatus::FOLLOWED);
        domain->setIP(ipAddress);
        return HTTPHandler::MessageStatus::OK;
    }
    return HTTPHandler::MessageStatus::NO_IN_DATABASE;
}
