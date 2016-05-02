#include "catch.hpp"
#include "../src/HTTPHandler.hpp"
#include "../src/Database.hpp"


SCENARIO("We want to send request with add domains and get respone")
{
    GIVEN("Request with add")
    {
        Database::getInstance().clear();
        std::string addRequest = "{\"command\":\"add\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]}";
        std::string expectedResponseToAddRequest = "{\"task\": {\"command\":\"add\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]},\"result\":[{\"domain\":\"google.com\", \"status\":\"ok\"}, {\"domain\":\"elka.pw.edu.pl\", \"status\": \"ok\"}]}";
        Json::Value jsonResponse1;
        Json::Reader reader1;
        reader1.parse(expectedResponseToAddRequest, jsonResponse1);
        WHEN("There are no domains in database and we initate action")
        {
            HTTPHandler handler;
            Json::Value response1 = handler.testAction(addRequest);
            THEN("We test if the response is correct")
            {
                REQUIRE(jsonResponse1==response1);
            }
        }
        WHEN("There are domains in database and we initate action")
        {
            Database::getInstance().addDomain("google.com");
            THEN("We try to add one domain again")
            {
                HTTPHandler handler;
                std::string secondAddRequest = "{\"command\":\"add\", \"domains\": [\"google.com\"]}";
                std::string secondExpectedResponseToAddRequest = "{\"task\": {\"command\":\"add\", \"domains\": [\"google.com\"]},\"result\":[{\"domain\":\"google.com\", \"status\":\"already_in_database\"}]}";
                Json::Value jsonResponse2;
                Json::Reader reader2;
                reader2.parse(secondExpectedResponseToAddRequest, jsonResponse2);
                Json::Value response2 = handler.testAction(secondAddRequest);
                REQUIRE(jsonResponse2==response2);
            }
        }
    }
}


SCENARIO("\"We want to send request with query and get response\"")
{
    GIVEN("Request with query")
    {
        Database::getInstance().clear();
        std::string queryRequest1 = "{\"command\":\"query\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]}";
        std::string expectedResponse1 = "{\"task\": {\"command\":\"query\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]},\"result\":[{\"domain\":\"google.com\", \"status\":\"no_in_database\"}, {\"domain\":\"elka.pw.edu.pl\", \"status\": \"no_in_database\"}]}";
        Json::Value jsonValue1;
        Json::Reader reader1;
        reader1.parse(expectedResponse1, jsonValue1);
        WHEN("There are no domains in database and we initiate query")
        {
            HTTPHandler handler1;
            Json::Value response1 = handler1.testAction(queryRequest1);
            THEN("We test if the response is correct")
            {
                REQUIRE(jsonValue1==response1);
            }
        }
        WHEN("We add domains, change status of one and initiate query")
        {
            Database::getInstance().addDomain("google.com");
            Database::getInstance().addDomain("elka.pw.edu.pl");
            Database::getInstance().updateDomain("google.com", Domain::DomainStatus::FOLLOWED);
            std::string request2 = "{\"command\":\"query\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]}";
            std::string expectedResponse2 = "{\"task\": {\"command\":\"query\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]},\"result\":[{\"domain\":\"google.com\", \"status\":\"ok\"}, {\"domain\":\"elka.pw.edu.pl\", \"status\": \"unknown\"}]}";
            HTTPHandler handler2;
            Json::Value jsonValue2;
            Json::Value response2 = handler2.testAction(queryRequest1);
            reader1.parse(expectedResponse2, jsonValue2);
            THEN("We test if the response is correct")
            {
                REQUIRE(jsonValue2==response2);
                //cleanup
                Database::getInstance().removeDomain("google.com");
                Database::getInstance().removeDomain("elka.pw.edu.pl");
            }
        }
    }
}


SCENARIO("\"We want to send request with remove domains and get response\"")
{
    GIVEN("Request with remove")
    {
        std::string request1 = "{\"command\":\"remove\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]}";
        std::string expectedResponse1 = "{\"task\": {\"command\":\"remove\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]},\"result\":[{\"domain\":\"google.com\", \"status\":\"no_in_database\"}, {\"domain\":\"elka.pw.edu.pl\", \"status\": \"no_in_database\"}]}";
        Json::Value jsonValue1;
        Json::Reader reader1;
        reader1.parse(expectedResponse1, jsonValue1);
        WHEN("There are no domains in database")
        {
            HTTPHandler handler1;
            Json::Value response1 = handler1.testAction(request1);
            THEN("We test if the response is correct")
            {
                REQUIRE(jsonValue1==response1);
            }
        }
        WHEN("There are domains in database")
        {
            Database::getInstance().addDomain("google.com");
            Database::getInstance().addDomain("elka.pw.edu.pl");
            std::string expectedResponse2 = "{\"task\": {\"command\":\"remove\", \"domains\": [\"google.com\", \"elka.pw.edu.pl\"]},\"result\":[{\"domain\":\"google.com\", \"status\":\"ok\"}, {\"domain\":\"elka.pw.edu.pl\", \"status\": \"ok\"}]}";
            Json::Value jsonValue2;
            HTTPHandler handler2;
            Json::Reader reader2;
            reader2.parse(expectedResponse2, jsonValue2);
            Json::Value response2 = handler2.testAction(request1);
            THEN("We test if the response is correct")
            {
                REQUIRE(jsonValue2==response2);
            }
        }
    }
}