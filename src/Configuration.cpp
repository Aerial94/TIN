#include "Configuration.hpp"
#include <json/json.h>

void Configuration::parse(std::string fileName) {
    std::ifstream infile("configuration.json");
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
    this->httpServerAddress = root["httpServer"]["address"].asString();
}

