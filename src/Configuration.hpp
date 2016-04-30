#ifndef DNS_CHECKER_CONFIGURATION_HPP
#define DNS_CHECKER_CONFIGURATION_HPP
#include <string>
#include "Logger.hpp"

class Configuration;

class ConfigurationException
{
    friend class Configuration;
    ConfigurationException(ConfigurationException::Type type): type(type) {}
    enum Type {
        NO_CONFIG_FILE,
        CONFIG_FILE_EMPTY,
        CONFIG_FILE_SYNTAX_ERROR,
        INCOMPLETE_CONFIG_FILE
    };
    Type type;
public:
    const std::string getDescription() const {
        return this->description;
    }

    void setDescription(const std::string description) {
        this->description = description;
    }

private:
    std::string description;
};

class Configuration {
public:
    Configuration() { }
    virtual ~Configuration() { }
    Configuration& getInstance();
private:
    void parse(std::string fileName);
    std::string httpServerAddress;
    /*Port is in little endian, must be converted to use in socket*/
    short httpServerPort;
    int readTimeout;
    int maxThreads;
    /*Interval in seconds*/
    int dnsPoolerInterval;
    Logger::LogLevel logLevel;
};


#endif //DNS_CHECKER_CONFIGURATION_HPP
