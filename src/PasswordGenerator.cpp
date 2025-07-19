#include "PasswordGenerator.h"
#include <stdexcept>

const char* PasswordGenerator::toString(PasswordType type) {
        switch(type) {
            case PasswordType::digits: return "0123456789";
            case PasswordType::letters: return "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
            case PasswordType::capitalLetters: return "QWERTYUIOPASDFGHJKLZXCVBNM";
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

    for (int i = 0; i < length; ++i) {
        password += allowedChars[charDist(rng)];
    }

    return password;
}