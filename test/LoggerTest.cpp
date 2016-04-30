#include "catch.hpp"
#include "../src/Logger.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>

SCENARIO("DEBUG Logger adds lines to file") {
    GIVEN("No log file and new Logger DEBUG instance") {
        std::remove("log.txt");
        Logger logger(Logger::LogLevel::DEBUG);
        WHEN("We ask for log Info") {
            logger.logInfo("testModuleName", "TestMessage");
            logger.close();
            THEN("file is created") {
                std::ifstream infile("log.txt",
                                     std::ios::ate);
                REQUIRE(infile.good() == true);
                AND_THEN("file size is not 0") {
                    infile.seekg(0, std::ios_base::end);
                    REQUIRE(infile.tellg() != 0);
                    AND_THEN("line contains INFO") {
                        infile.seekg(0, std::ios_base::beg);
                        std::string sLine;
                        std::getline(infile, sLine);
                        REQUIRE(sLine.find("[INFO]") != std::string::npos);
                    }
                }
            }
        }
        WHEN("We ask for log Warning") {
            logger.logWarning("testModuleName", "TestMessage");
            logger.close();
            THEN("file is created") {
                std::ifstream infile("log.txt",
                                     std::ios::ate);
                REQUIRE(infile.good() == true);
                AND_THEN("file size is not 0") {
                    infile.seekg(0, std::ios_base::end);
                    REQUIRE(infile.tellg() != 0);
                    AND_THEN("line contains WARNING") {
                        infile.seekg(0, std::ios_base::beg);
                        std::string sLine;
                        std::getline(infile, sLine);
                        REQUIRE(sLine.find("[WARNING]") != std::string::npos);
                    }
                }
            }
        }
        WHEN("We ask for log Debug") {
            logger.logDebug("testModuleName", "TestMessage");
            logger.close();
            THEN("file is created") {
                std::ifstream infile("log.txt",
                                     std::ios::ate);
                REQUIRE(infile.good() == true);
                AND_THEN("file size is not 0") {
                    infile.seekg(0, std::ios_base::end);
                    REQUIRE(infile.tellg() != 0);
                    AND_THEN("line contains DEBUG") {
                        infile.seekg(0, std::ios_base::beg);
                        std::string sLine;
                        std::getline(infile, sLine);
                        REQUIRE(sLine.find("[DEBUG]") != std::string::npos);
                    }
                }
            }
        }
    }
}
