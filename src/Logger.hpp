#ifndef DNS_CHECKER_LOGGER_HPP
#define DNS_CHECKER_LOGGER_HPP

#include <ctime>
#include <fstream>
#include <mutex>
#include <string>

class Logger
{
  public:
    Logger();
    enum LogLevel { NONE, INFO, WARNING, DEBUG };
    Logger(LogLevel logLevel);
    ~Logger();
    static Logger &getInstance();
    void logInfo(std::string moduleName, std::string message);
    void logWarning(std::string moduleName, std::string message);
    void logDebug(std::string moduleName, std::string message);
    void setLogLevel(LogLevel logLevel);
    void stop();
    void flush();
    void close();

  private:
    bool disabled;
    std::mutex mutex;
    LogLevel logLevel;
    std::ofstream logFile;
    std::string getCurrentTimeDate();

    void appendToLogFile(std::string level, std::string moduleName,
                         std::string message);
};

#endif
