#include "catch.hpp"
#include "../src/Database.hpp"

SCENARIO("We want to add new domain to database and then remove it")
{
    GIVEN("Name of domain to add")
    {
        std::string domainName = "google.com";
        WHEN("We create Database and an instance of Domain")
        {
            Domain domain(domainName);
            THEN("We add the domain")
            {
                HTTPHandler::MessageStatus status = Database::getInstance().addDomain(domainName);
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                    AND_THEN("We try to add already existing domain")
                    {
                        status = Database::getInstance().addDomain(domainName);
                        REQUIRE(status == HTTPHandler::MessageStatus::ALREADY_IN_DATABASE);
                        AND_THEN("We remove it")
                        {
                            status = Database::getInstance().removeDomain(domainName);
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
            THEN("We try to get status")
            {
                HTTPHandler::MessageStatus status = Database::getInstance().getDomainStatus(domainName);
                REQUIRE(status == HTTPHandler::MessageStatus::NO_IN_DATABASE);
                AND_THEN("Now we add domain and ask about status")
                {
                    Database::getInstance().addDomain(domainName);
                    status = Database::getInstance().getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::UNKNOWN);
                    Database::getInstance().removeDomain(domainName);
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
        Database::getInstance().addDomain(domainName);
        WHEN("We update status to FOLLOWED")
        {
            status = Database::getInstance().updateDomain(domainName, Domain::DomainStatus::FOLLOWED);
            THEN("We check the message status")
            {
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                AND_THEN("Now we check domain status")
                {
                    status = Database::getInstance().getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::OK);
                }
            }
        }
        WHEN("We update status to NONEXISTENT")
        {
            status = Database::getInstance().updateDomain(domainName, Domain::DomainStatus::NONEXISTENT);
            THEN("We check the message status")
            {
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                AND_THEN("Now we check domain status")
                {
                    status = Database::getInstance().getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::UNREACHABLE);
                }
            }
        }
        WHEN("We update status to FOLLOWED_BUT_UNCHECKED")
        {
            status = Database::getInstance().updateDomain(domainName, Domain::DomainStatus::FOLLOWED_BUT_UNCHECKED);
            THEN("We check the message status")
            {
                REQUIRE(status == HTTPHandler::MessageStatus::OK);
                AND_THEN("Now we check domain status")
                {
                    status = Database::getInstance().getDomainStatus(domainName);
                    REQUIRE(status == HTTPHandler::MessageStatus::UNKNOWN);
                    Database::getInstance().removeDomain(domainName);
                }
            }
        }
    }
}


SCENARIO("We want to iterate through domains")
{
    GIVEN("Names of domains to add")
    {
        std::string domainsToAdd[] = {"google.com", "elka.pw.edu.pl", "raz.dwa.trzy.pl", "cztery.net"};
        WHEN("We add domains to database")
        {
            int i;
            for(i = 0; i < domainsToAdd->length(); ++i)
                Database::getInstance().addDomain(domainsToAdd[i]);
            THEN("We test status")
            {
                std::string currentDomain = Database::getInstance().getNextDomain();
                int j;
                bool flag = false;
                for(j = 0;  currentDomain != ""; currentDomain = Database::getInstance().getNextDomain(), ++j)
                {
                    if(currentDomain != domainsToAdd[j])
                    {
                        flag = true;
                        break;
                    }
                }
                REQUIRE(flag == false);
            }
        }
    }
}