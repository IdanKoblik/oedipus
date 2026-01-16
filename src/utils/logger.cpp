#include "utils/logger.h"
#include <iostream>
#include <ctime>
#include <cstring>

namespace logger {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& logFile) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (initialized_) {
            return;
        }
        
        logFile_.open(logFile, std::ios::app);
        if (!logFile_.is_open()) {
            std::cerr << "WARNING: Cannot open log file: " << logFile << std::endl;
            initialized_ = false;
            return;
        }
        
        initialized_ = true;
    }

    log(Level::INFO, "Logger initialized", __FILE__, __LINE__);
}

void Logger::shutdown() {
    bool wasInitialized = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        wasInitialized = initialized_;
        if (initialized_) {
            initialized_ = false;
        }
    }
    
    // Release lock before logging to avoid deadlock
    if (wasInitialized) {
        log(Level::INFO, "Logger shutting down", __FILE__, __LINE__);
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
}

Logger::~Logger() {
    shutdown();
}

void Logger::log(Level level, const std::string& message, 
                 const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << getTimestamp() << " [" << levelToString(level) << "]";
    
    if (!file.empty() && line > 0) {
        // Extract just the filename from the full path
        size_t lastSlash = file.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos) 
            ? file.substr(lastSlash + 1) 
            : file;
        oss << " [" << filename << ":" << line << "]";
    }
    
    oss << " " << message;
    
    std::string logMessage = oss.str();
    writeLog(logMessage);
}

void Logger::logException(const std::exception& e, 
                          const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << getTimestamp() << " [EXCEPTION]";
    
    if (!file.empty() && line > 0) {
        size_t lastSlash = file.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos) 
            ? file.substr(lastSlash + 1) 
            : file;
        oss << " [" << filename << ":" << line << "]";
    }
    
    oss << " Exception caught: " << e.what();
    
    std::string logMessage = oss.str();
    writeLog(logMessage);
}

void Logger::logUnknownException(const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << getTimestamp() << " [EXCEPTION]";
    
    if (!file.empty() && line > 0) {
        size_t lastSlash = file.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos) 
            ? file.substr(lastSlash + 1) 
            : file;
        oss << " [" << filename << ":" << line << "]";
    }
    
    oss << " Unknown exception caught (not derived from std::exception)";
    
    std::string logMessage = oss.str();
    writeLog(logMessage);
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::DEBUG: return "DEBUG";
        case Level::INFO:  return "INFO ";
        case Level::WARN:  return "WARN ";
        case Level::ERROR: return "ERROR";
        case Level::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

void Logger::writeLog(const std::string& logMessage) {
    std::cerr << logMessage << std::endl;
    
    if (initialized_ && logFile_.is_open()) {
        logFile_ << logMessage << std::endl;
        logFile_.flush(); // Ensure immediate write
    }
}

} 

