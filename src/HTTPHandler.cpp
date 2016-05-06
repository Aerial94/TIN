#include "HTTPHandler.hpp"
#include "Database.hpp"
#include "Logger.hpp"

Json::Value HTTPHandler::testAction(std::string &json)
{
    chooseAction(json);
    return this->response;
}

void HTTPHandler::parse(std::string json)
{
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(json, root);

    if (!parsingSuccessful)
    {
        std::string details = reader.getFormattedErrorMessages();
        HTTPHandlerException httpHandlerException(HTTPHandlerException::Type::REQUEST_SYNTAX_ERROR);
        httpHandlerException.setDescription(details);
        Logger::getInstance().logWarning("HTTPHandler", "Invalid input json");
        throw httpHandlerException;
    }

    if(!root["command"].empty())
        this->command = root["command"].asString();
    else
    {
        HTTPHandlerException httpHandlerException(HTTPHandlerException::Type::REQUEST_NO_COMMAND);
        httpHandlerException.setDescription("No command was specified in json file");
        Logger::getInstance().logWarning("HTTPHandler", "Invalid input json");
        throw httpHandlerException;
    }

    if(!root["domains"].empty())
    {
        Json::Value jsonDomains = root["domains"];
        for(int i = 0; i < jsonDomains.size(); ++i)
            this->domains.push_back(jsonDomains[i].asString());
    }
    else
    {
        HTTPHandlerException httpHandlerException(HTTPHandlerException::Type::REQUEST_NO_DOMAINS);
        httpHandlerException.setDescription("No domains were specified in json file");
        Logger::getInstance().logWarning("HTTPHandler", "Invalid input json");
        throw httpHandlerException;
    }
}

void HTTPHandler::chooseAction(std::string &json)
{
    parse(json);
    std::string domainNames = getAllDomainNames();
    if(this->command == addAction)
    {
        for(std::vector<std::string>::iterator it = this->domains.begin(); it != this->domains.end(); ++it)
        {
            this->vecOfDomains.append(*it);
            this->pair["domain"] = *it;
            HTTPHandler::MessageStatus status = Database::getInstance().addDomain(*it);
            this->pair["status"] = statusToString(status);
            this->vecOfDomainStatusPairs.append(this->pair);
        }
        Logger::getInstance().logInfo("HTTPHandler", "The following domains were added: " + domainNames);
    }
    else if(this->command == removeAction)
    {
        for(std::vector<std::string>::iterator it = this->domains.begin(); it != this->domains.end(); ++it)
        {
            this->vecOfDomains.append(*it);
            this->pair["domain"] = *it;
            HTTPHandler::MessageStatus status = Database::getInstance().removeDomain(*it);
            this->pair["status"] = statusToString(status);
            this->vecOfDomainStatusPairs.append(this->pair);
            Logger::getInstance().logInfo("HTTPHandler", "The following domains were removed: " + domainNames);
        }
    }
    else if(this->command == queryAction)
    {
        for(std::vector<std::string>::iterator it = this->domains.begin(); it != this->domains.end(); ++it)
        {
            this->vecOfDomains.append(*it);
            this->pair["domain"] = *it;
            HTTPHandler::MessageStatus status = Database::getInstance().getDomainStatus(*it);
            this->pair["status"] = statusToString(status);
            this->vecOfDomainStatusPairs.append(this->pair);
            Logger::getInstance().logInfo("HTTPHandler", "The following domains were queued: " + domainNames);
        }
    }
    prepareResponse();
}

std::string HTTPHandler::statusToString(HTTPHandler::MessageStatus s)
{
    if (s == HTTPHandler::MessageStatus::OK)
        return "ok";
    else if (s == HTTPHandler::MessageStatus::ALREADY_IN_DATABASE)
        return "already_in_database";
    else if (s == HTTPHandler::MessageStatus::NO_IN_DATABASE)
        return "no_in_database";
    else if (s == HTTPHandler::MessageStatus::UNKNOWN)
        return "unknown";
    else if (s == HTTPHandler::MessageStatus::UNREACHABLE)
        return "unreachable";
}

void HTTPHandler::prepareResponse()
{
    this->response["task"]["command"] = this->command;
    this->response["task"]["domains"] = this->vecOfDomains;
    this->response["result"] = vecOfDomainStatusPairs;
}

std::string HTTPHandler::getAllDomainNames()
{
    std::string domainNames = "";
    for(std::vector<std::string>::iterator it = this->domains.begin(); it != this->domains.end(); ++it)
    {
        domainNames += *it;
        domainNames += ", ";
    }
    domainNames.pop_back();
    domainNames.pop_back();
    return domainNames;
}

std::string HTTPHandler::getResponse(std::string json)
{
    chooseAction(json);
    Json::FastWriter writer;
    return writer.write(this->response);
}