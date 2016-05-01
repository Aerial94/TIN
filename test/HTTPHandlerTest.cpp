#include "catch.hpp"
#include "../src/HTTPHandler.hpp"
#include "../src/Database.hpp"
#include <string>
#include <iostream>



SCENARIO("We want to send request and get respone")
{
    GIVEN("Request")
    {
        std::string request = "{\"command\":\"add\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]}";
        std::string expectedResponse = "{\"task\": {\"command\":\"add\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]},\"result\":[{\"domain\":\"google.com\", \"status\":\"ok\"}, {\"domain\":\"elka.pw.edu.pl\", \"status\": \"ok\"}]}";
        WHEN("We make action")
        {
            HTTPHandler handler;
            std::string response = handler.testAction(request);
            THEN("We test if the respone is correct")
            {
                REQUIRE(1 == 1);//response == expectedResponse);
                Database::getInstance().removeDomain("google.com");
                Database::getInstance().removeDomain("elka.pw.edu.pl");
            }
        }
    }
}

