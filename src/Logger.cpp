#include "Logger.hpp"

Logger::Logger(LogLever logLever) : logLever(logLever) {
    this->logFile.open("log.txt", std::ios_base::app);
}

void Logger::logInfo(std::string moduleName, std::string message) {
    if (this->logLever < LogLever::INFO)
        return;
    this->appendToLogFile("[INFO]", moduleName, message);
}

void Logger::logWarning(std::string moduleName, std::string message) {
    if (this->logLever < LogLever::WARNING)
        return;
    this->appendToLogFile("[WARNING]", moduleName, message);
}

void Logger::logDebug(std::string moduleName, std::string message) {
    if (this->logLever < LogLever::DEBUG)
        return;
    this->appendToLogFile("[DEBUG]", moduleName, message);

}

Logger::~Logger() {
    this->close();
}

void Logger::appendToLogFile(std::string level, std::string moduleName,
                             std::string message) {
    this->logFile << this->getCurrentTimeDate();
    this->logFile << " ";
    this->logFile << level;
    this->logFile << " ";
    this->logFile << "(" << moduleName << ")";
    this->logFile << " ";
    this->logFile << message;
    this->logFile << "\n";
}

std::string Logger::getCurrentTimeDate() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[1024];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
    std::string str(buffer);
    return str;
}

void Logger::flush() {
    this->logFile.flags();
}

void Logger::close() {
    this->logFile.close();
}




