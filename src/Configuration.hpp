#ifndef DNS_CHECKER_CONFIGURATION_HPP
#define DNS_CHECKER_CONFIGURATION_HPP
#include <string>
#include <json/json.h>
#include "Logger.hpp"

class Configuration;

class ConfigurationException
{
public:
    enum Type {
        NO_CONFIG_FILE,
        CONFIG_FILEUXP1A_EMPTY,
        CONFIG_FILE_SYNTAX_ERROR,
        INCOMPLETE_CONFIG_FILE
    };
    const std::string getDescription() const {
        return this->description;
    }
    void setDescription(const std::string description) {
        this->description = description;
    }
private:
    std::string description;
    friend class Configuration;
    ConfigurationException(ConfigurationException::Type type) {
        this->type = type;
        if (this->type == NO_CONFIG_FILE)
            this->description = "Config file not found";
    }
    Type type;
};

class Configuration {
public:
    Configuration();
    virtual ~Configuration() { }
    static Configuration& getInstance();

    short getHttpServerPort() const {
        return httpServerPort;
    }

    std::string getHttpServerAddress() const {
        return httpServerAddress;
    }

    int getHttpServerReadTimeout() const {
        return httpServerReadTimeout;
    }

    int getHttpServerMaxThreads() const {
        return httpServerMaxThreads;
    }

    int getDnsPoolerInterval() const {
        return dnsPoolerInterval;
    }

    Logger::LogLevel getLogLevel() const {
        return logLevel;
    }
    bool isReadyToUse() const {
        return readyToUse;
    }

    void parse(std::string fileName);
    void parse();
private:
    bool readyToUse;
    std::string httpServerAddress;
    /*Port is in little endian, must be converted to use in socket*/
    short httpServerPort;
    int httpServerReadTimeout;
    int httpServerMaxThreads;
    /*Interval in seconds*/
    int dnsPoolerInterval;
    Logger::LogLevel logLevel;

    void parseLogLevel(const Json::Value &root);
};


#endif //DNS_CHECKER_CONFIGURATION_HPP
