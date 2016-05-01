#include "catch.hpp"
#include "../src/Domain.hpp"
#include <string>
#include <iostream>

SCENARIO("We want to create new domain with a given name")
{
    GIVEN("Name of domain to create")
    {
        std::string domainName = "google.com";
        WHEN("We create new instance of Domain")
        {
            Domain domain(domainName);
                THEN("We test if the name is correct")
                {
                    REQUIRE(domain.getDomainName() == domainName);
                    AND_THEN("We test if status is correct")
                    {
                        REQUIRE(domain.getStatus() == Domain::DomainStatus::FOLLOWED_BUT_UNCHECKED);
                    }
                }
        }
    }
}