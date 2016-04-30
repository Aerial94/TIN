#ifndef DNS_CHECKER_LOGGER_HPP
#define DNS_CHECKER_LOGGER_HPP

#include <string>
#include <fstream>
#include <ctime>

class Logger {
public:
    enum LogLever {NONE, INFO, WARNING, DEBUG};
    Logger(LogLever logLever);
    ~Logger();
    void logInfo(std::string moduleName, std::string message);
    void logWarning(std::string moduleName, std::string message);
    void logDebug(std::string moduleName, std::string message);
private:
    LogLever logLever;
    std::ofstream logFile;
private:
    std::string getCurrentTimeDate();
    void appendToLogFile(std::string level, std::string moduleName,
                         std::string message);
};


#endif
