#include "Configuration.hpp"
#include <json/json.h>
#include "Logger.hpp"

void Configuration::parse() {
    this->parse("configuration.json");
}

void Configuration::parse(std::string fileName) {
    std::ifstream infile(fileName);
    //check if file exists
    if (not infile.good()) {
        infile.close();
        throw ConfigurationException(ConfigurationException::Type::NO_CONFIG_FILE);
    }
    Json::Value root;
    Json::Reader reader;
    bool parseOK = reader.parse(infile,root, false);
    if (not parseOK) {
        std::string details = reader.getFormattedErrorMessages();
        ConfigurationException configurationException(ConfigurationException::Type::CONFIG_FILE_SYNTAX_ERROR);
        configurationException.setDescription(details);
        infile.close();
        throw configurationException;
    }
    if (not root["httpServer"]["address"].empty())
        this->httpServerAddress = root["httpServer"]["address"].asString();
    else {
        ConfigurationException exception = ConfigurationException(
                ConfigurationException::Type::INCOMPLETE_CONFIG_FILE);
        exception.setDescription("httpServer.address not provided in config file");
        throw exception;
    }
    if (not root["httpServer"]["port"].empty())
        this->httpServerPort = root["httpServer"]["port"].asInt();
    else {
        ConfigurationException exception = ConfigurationException(
                ConfigurationException::Type::INCOMPLETE_CONFIG_FILE);
        exception.setDescription("httpServer.port not provided in config file");
        throw exception;
    }
    if (not root["httpServer"]["readTimeout"].empty())
        this->httpServerReadTimeout = root["httpServer"]["readTimeout"].asInt();
    else {
        ConfigurationException exception = ConfigurationException(
                ConfigurationException::Type::INCOMPLETE_CONFIG_FILE);
        exception.setDescription("httpServer.readTimeout not provided in config file");
        throw exception;
    }
    if (not root["httpServer"]["maxThreads"].empty())
        this->httpServerMaxThreads = root["httpServer"]["maxThreads"].asInt();
    else {
        ConfigurationException exception = ConfigurationException(
                ConfigurationException::Type::INCOMPLETE_CONFIG_FILE);
        exception.setDescription("httpServer.maxThreads not provided in config file");
        throw exception;
    }
    if (not root["dnsPooler"]["interval"].empty())
        this->dnsPoolerInterval = root["dnsPooler"]["interval"].asInt();
    else {
        ConfigurationException exception = ConfigurationException(
                ConfigurationException::Type::INCOMPLETE_CONFIG_FILE);
        exception.setDescription("dnsPooler.interval not provided in config file");
        throw exception;
    }
    parseLogLevel(root);
    this->readyToUse = true;
}

void Configuration::parseLogLevel(const Json::Value &root) {
    if (root["LogLevel"].asString() == "INFO"){
        logLevel = Logger::INFO;
    }
    else if (root["logLevel"].asString() == "WARNING"){
        logLevel = Logger::WARNING;
    }
    else if (root["logLevel"].asString() == "DEBUG"){
        logLevel = Logger::DEBUG;
    }
    else if (root["logLevel"].asString() == "NONE"){
        logLevel = Logger::NONE;
    }
    else {
        ConfigurationException exception = ConfigurationException(
                ConfigurationException::INCOMPLETE_CONFIG_FILE);
        exception.setDescription("Unkonwn logLevel type " + root["logLevel"].asString());
        throw exception;
    }
}

Configuration &Configuration::getInstance() {
    static Configuration configuration;
    return configuration;
}

Configuration::Configuration() {
    this->readyToUse = false;
}






