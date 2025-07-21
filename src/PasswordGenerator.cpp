#include "PasswordGenerator.h"
#include <stdexcept>

const char* PasswordGenerator::toString(PasswordType type) {
        switch(type) {
            case PasswordType::digits: return "0123456789";
            case PasswordType::letters: return "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
            case PasswordType::uppercaseLetters: return "QWERTYUIOPASDFGHJKLZXCVBNM";
            case PasswordType::lowercaseLetters: return "qwertyuiopasdfghjklzxcvbnm";
            case PasswordType::symbols: return "!@#$%&*";
            case PasswordType::digitsAndLetters: return "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm0123456789";
            case PasswordType::allChars: return "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm0123456789!@#$%&*";
            default: return "";
        }
    }

PasswordGenerator::PasswordGenerator(const std::string& chars, int minLen, int maxLen)
    : allowedChars(chars),
      minLength(minLen),
      maxLength(maxLen),
      rng(std::random_device{}())
{
    if (allowedChars.empty())
        throw std::invalid_argument("Allowed character set is empty.");

    if (minLength <= 0 || maxLength <= 0 || minLength > maxLength)
        throw std::invalid_argument("Invalid password length range.");
}

PasswordGenerator::PasswordGenerator(PasswordType type, int minLen, int maxLen)
    : PasswordGenerator(toString(type), minLen, maxLen) {}

std::string PasswordGenerator::operator()() {
    std::uniform_int_distribution<int> lenDist(minLength, maxLength);
    std::uniform_int_distribution<size_t> charDist(0, allowedChars.size() - 1);

    int length = lenDist(rng);
    std::string password;
    password.reserve(length);

    int j = 0;
    if (allowedChars == toString(PasswordType::allChars)) {
        const std::string digits = toString(PasswordType::digits);
        std::uniform_int_distribution<size_t> digitsDist(0, digits.size() - 1);
        password += digits[digitsDist(rng)];
        ++j;
        const std::string uppercaseLetters = toString(PasswordType::uppercaseLetters);
        std::uniform_int_distribution<size_t> uppercaseLettersDist(0, uppercaseLetters.size() - 1);
        password += uppercaseLetters[uppercaseLettersDist(rng)];
        ++j;
        const std::string lowercaseLetters = toString(PasswordType::lowercaseLetters);
        std::uniform_int_distribution<size_t> lowercaseLettersDist(0, lowercaseLetters.size() - 1);
        password += lowercaseLetters[lowercaseLettersDist(rng)];
        ++j;
        const std::string symbols = toString(PasswordType::symbols);
        std::uniform_int_distribution<size_t> symbolsDist(0, symbols.size() - 1);
        password += symbols[symbolsDist(rng)];
        ++j;
    }

    if (allowedChars == toString(PasswordType::digitsAndLetters)) {
        const std::string digits = toString(PasswordType::digits);
        std::uniform_int_distribution<size_t> digitsDist(0, digits.size() - 1);
        password += digits[digitsDist(rng)];
        ++j;
        const std::string uppercaseLetters = toString(PasswordType::uppercaseLetters);
        std::uniform_int_distribution<size_t> uppercaseLettersDist(0, uppercaseLetters.size() - 1);
        password += uppercaseLetters[uppercaseLettersDist(rng)];
        ++j;
        const std::string lowercaseLetters = toString(PasswordType::lowercaseLetters);
        std::uniform_int_distribution<size_t> lowercaseLettersDist(0, lowercaseLetters.size() - 1);
        password += lowercaseLetters[lowercaseLettersDist(rng)];
        ++j;
    }

    if (allowedChars == toString(PasswordType::letters)) {
        const std::string uppercaseLetters = toString(PasswordType::uppercaseLetters);
        std::uniform_int_distribution<size_t> uppercaseLettersDist(0, uppercaseLetters.size() - 1);
        password += uppercaseLetters[uppercaseLettersDist(rng)];
        ++j;
        const std::string lowercaseLetters = toString(PasswordType::lowercaseLetters);
        std::uniform_int_distribution<size_t> lowercaseLettersDist(0, lowercaseLetters.size() - 1);
        password += lowercaseLetters[lowercaseLettersDist(rng)];
        ++j;
    }

    for (int i = j; i < length; ++i) {
        password += allowedChars[charDist(rng)];
    }

    std::shuffle(password.begin(), password.end(), rng);

    return password;
}