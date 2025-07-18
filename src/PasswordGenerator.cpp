#include "PasswordGenerator.h"
#include <stdexcept>

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