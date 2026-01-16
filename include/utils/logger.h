#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace logger {

enum class Level {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger {
public:
    static Logger& instance();
    
    void init(const std::string& logFile = "logs.txt");
    void shutdown();
    
    void log(Level level, const std::string& message, 
             const std::string& file = "", int line = 0);
    
    void logException(const std::exception& e, 
                      const std::string& file = "", int line = 0);
    
    void logUnknownException(const std::string& file = "", int line = 0);

private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::string levelToString(Level level);
    std::string getTimestamp();
    void writeLog(const std::string& logMessage);
    
    std::ofstream logFile_;
    std::mutex mutex_;
    bool initialized_ = false;
};

// Convenience macros
#define LOG_DEBUG(msg) logger::Logger::instance().log(logger::Level::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) logger::Logger::instance().log(logger::Level::INFO, msg, __FILE__, __LINE__)
#define LOG_WARN(msg) logger::Logger::instance().log(logger::Level::WARN, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) logger::Logger::instance().log(logger::Level::ERROR, msg, __FILE__, __LINE__)
#define LOG_FATAL(msg) logger::Logger::instance().log(logger::Level::FATAL, msg, __FILE__, __LINE__)

#define LOG_EXCEPTION(e) logger::Logger::instance().logException(e, __FILE__, __LINE__)
#define LOG_UNKNOWN_EXCEPTION() logger::Logger::instance().logUnknownException(__FILE__, __LINE__)

}

#endif // LOGGER_H

