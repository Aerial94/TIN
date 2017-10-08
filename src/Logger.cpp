#include "Logger.hpp"
#include <iostream>

Logger::Logger(LogLevel logLevel) : logLevel(logLevel) {
    this->logFile.open("log.txt", std::ios_base::app);
}

void Logger::logInfo(std::string moduleName, std::string message) {
    if (this->logLevel < LogLevel::INFO)
        return;
    std::lock_guard<std::mutex> guard(this->mutex);
    this->appendToLogFile("[INFO]", moduleName, message);
}

void Logger::logWarning(std::string moduleName, std::string message) {
    if (this->logLevel < LogLevel::WARNING)
        return;
    std::lock_guard<std::mutex> guard(this->mutex);
    this->appendToLogFile("[WARNING]", moduleName, message);
}

void Logger::logDebug(std::string moduleName, std::string message) {
    if (this->logLevel < LogLevel::DEBUG)
        return;
    std::lock_guard<std::mutex> guard(this->mutex);
    this->appendToLogFile("[DEBUG]", moduleName, message);
}

Logger::~Logger() {
    this->close();
}

/*
 * WARNING: this function must be called after acquiring the lock of mutex
 */
void Logger::appendToLogFile(std::string level, std::string moduleName,
                             std::string message) {
    if (not disabled) {
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
    std::cout << this->getCurrentTimeDate();
    std::cout << " ";
    std::cout << level;
    std::cout << " ";
    std::cout << "(" << moduleName << ")";
    std::cout << " ";
    std::cout << message;
    std::cout << "\n";
}

std::string Logger::getCurrentTimeDate() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[1024];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);
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
    this->disabled = false;
    this->logFile.open("log.txt", std::ios_base::app);
    this->logLevel = LogLevel::NONE;
}

void Logger::setLogLevel(LogLevel logLevel) {
    this->logLevel = logLevel;
}

void Logger::stop() {
    std::lock_guard<std::mutex> guard(this->mutex);
    this->appendToLogFile("[INFO]", "Exit", "Closing log file...");
    this->disabled = true;
    this->close();
}
