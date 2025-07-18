#pragma once

#include <string>
#include <random>

class PasswordGenerator {
    std::string allowedChars;
    int minLength;
    int maxLength;
    std::mt19937 rng;

public:
    PasswordGenerator (const std::string& chars, int minLen, int maxLen);
    
    std::string operator()();
};