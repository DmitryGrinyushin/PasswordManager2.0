#pragma once

#include <iostream>
#include <fstream>
#include <string>


enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
    static Logger* instance;
    std::ofstream logFile;

    Logger ();
    std::string getCurrentTime();

public:
    Logger (const Logger&) = delete;
    Logger& operator= (const Logger&) = delete;

    static Logger& getInstance();

    static const char* toString(LogLevel level);
    void log(LogLevel level, const std::string& message);

    ~Logger ();
};