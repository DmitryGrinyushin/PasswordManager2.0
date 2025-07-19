#pragma once

#include <string>
#include <random>

enum class PasswordType {
    digits,
    letters,
    capitalLetters,
    symbols,
    digitsAndLetters,
    allChars
};

class PasswordGenerator {
    std::string allowedChars;
    int minLength;
    int maxLength;
    std::mt19937 rng;
    
    static const char* toString(PasswordType type);

public:
    PasswordGenerator (const std::string& chars, int minLen, int maxLen);
    PasswordGenerator(PasswordType type, int minLen, int maxLen);
    
    std::string operator()();
};