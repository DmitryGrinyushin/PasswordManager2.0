#pragma once

#include <cstdlib>
#include <string>

inline std::string getEnvOrDefault(const std::string& var, const std::string& defaultValue) {
    const char* value = std::getenv(var.c_str());
    return (value && *value) ? std::string(value) : defaultValue;
}