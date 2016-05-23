#ifndef DNS_CHECKER_LOGGER_HPP
#define DNS_CHECKER_LOGGER_HPP

#include <string>
#include <fstream>
#include <ctime>
#include <mutex>

class Logger {
public:
    Logger();
    enum LogLevel {NONE, INFO, WARNING, DEBUG};
    Logger(LogLevel logLevel);
    ~Logger();
    static Logger& getInstance();
    void logInfo(std::string moduleName, std::string message);
    void logWarning(std::string moduleName, std::string message);
    void logDebug(std::string moduleName, std::string message);
    void setLogLevel(LogLevel logLevel);
    void flush();
    void close();
private:
    std::mutex mutex;
    LogLevel logLevel;
    std::ofstream logFile;
    std::string getCurrentTimeDate();

    void appendToLogFile(std::string level, std::string moduleName,
                         std::string message);
};


#endif
