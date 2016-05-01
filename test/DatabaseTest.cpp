#include "catch.hpp"
#include "../src/Database.hpp"
#include "../src/HTTPHandler.hpp"
#include <string>

SCENARIO("We want to add new domain to database and then remove it")
{
    GIVEN("Name of domain to add")
    {
        std::string domainName = "google.com";
        WHEN("We create Database and an instance of Domain")
        {
            Domain domain(domainName);
            Database database = Database::getInstance();
            THEN("We add the domain")
            {
                HTTPHandler::MessageStatus status = database.addDomain(domainName);
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                    AND_THEN("We try to add already existing domain")
                    {
                        status = database.addDomain(domainName);
                        REQUIRE(status == HTTPHandler::MessageStatus::ALREADY_IN_DATABASE);
                        AND_THEN("We remove it")
                        {
                            status = database.removeDomain(domainName);
                            REQUIRE(status == HTTPHandler::MessageStatus::OK);
                        }
                    }
            }
        }
    }
}

SCENARIO("We want to get status of given domain")
{
    GIVEN("Name of domain to check")
    {
        std::string domainName = "google.com";
        WHEN("We create Database and an instance of Domain")
        {
            Domain domain(domainName);
            Database database = Database::getInstance();
            THEN("We try to get status")
            {
                HTTPHandler::MessageStatus status = database.getDomainStatus(domainName);
                REQUIRE(status == HTTPHandler::MessageStatus::NO_IN_DATABASE);
                AND_THEN("Now we add domain and ask about status")
                {
                    database.addDomain(domainName);
                    status = database.getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::UNKNOWN);
                    database.removeDomain(domainName);
                }
            }
        }
    }
}


SCENARIO("We want to update status of domain")
{
    GIVEN("Database with domain in it")
    {
        HTTPHandler::MessageStatus status;
        std::string domainName = "google.com";
        Domain domain(domainName);
        Database database = Database::getInstance();
        database.addDomain(domainName);
        WHEN("We update status to FOLLOWED")
        {
            status = database.updateDomain(domainName, Domain::DomainStatus::FOLLOWED);
            THEN("We check the message status")
            {
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                AND_THEN("Now we check domain status")
                {
                    status = database.getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::OK);
                }
            }
        }
        WHEN("We update status to NONEXISTENT")
        {
            status = database.updateDomain(domainName, Domain::DomainStatus::NONEXISTENT);
            THEN("We check the message status")
            {
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                AND_THEN("Now we check domain status")
                {
                    status = database.getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::UNREACHABLE);
                }
            }
        }
        WHEN("We update status to FOLLOWED_BUT_UNCHECKED")
        {
            status = database.updateDomain(domainName, Domain::DomainStatus::FOLLOWED_BUT_UNCHECKED);
            THEN("We check the message status")
            {
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                AND_THEN("Now we check domain status")
                {
                    status = database.getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::UNKNOWN);
                }
            }
        }
    }
}