#include "Logger.hpp"

Logger::Logger(LogLevel logLevel) : logLevel(logLevel) {
    this->logFile.open("log.txt", std::ios_base::app);
}

void Logger::logInfo(std::string moduleName, std::string message) {
    if (this->logLevel < LogLevel::INFO)
        return;
    this->appendToLogFile("[INFO]", moduleName, message);
}

void Logger::logWarning(std::string moduleName, std::string message) {
    if (this->logLevel < LogLevel::WARNING)
        return;
    this->appendToLogFile("[WARNING]", moduleName, message);
}

void Logger::logDebug(std::string moduleName, std::string message) {
    if (this->logLevel < LogLevel::DEBUG)
        return;
    this->appendToLogFile("[DEBUG]", moduleName, message);

}

Logger::~Logger() {
    this->close();
}

void Logger::appendToLogFile(std::string level, std::string moduleName,
                             std::string message) {
    std::lock_guard<std::mutex> guard(this->mutex);
    this->logFile << this->getCurrentTimeDate();
    this->logFile << " ";
    this->logFile << level;
    this->logFile << " ";
    this->logFile << "(" << moduleName << ")";
    this->logFile << " ";
    this->logFile << message;
    this->logFile << "\n";
    this->logFile.flush();
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

Logger &Logger::getInstance() {
    static Logger logger;
    return logger;
}


Logger::Logger() {
    this->logFile.open("log.txt", std::ios_base::app);
    this->logLevel = LogLevel::NONE;
}

void Logger::setLogLevel(LogLevel logLevel) {
    this->logLevel = logLevel;
}

