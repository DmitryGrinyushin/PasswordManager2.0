#include "Logger.h"
#include "EnvUtils.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Logger* Logger::instance = nullptr;

Logger& Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return *instance;
}

Logger::Logger () {
    std::string logPath = getEnvOrDefault("PASSWORD_MANAGER_LOG_PATH", "log.txt");
    logFile.open(logPath, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Cannot open logfile");
    }
}

std::string Logger::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

const char* Logger::toString(LogLevel level) {
    switch(level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    logFile << "[" << getCurrentTime() << "] [" << toString(level) << "] " << message << std::endl;
}

Logger::~Logger () {
    logFile.close();
}