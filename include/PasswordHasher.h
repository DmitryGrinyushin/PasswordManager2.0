#pragma once
#include <string>

namespace PasswordHasher {
    std::string generateSalt(size_t length = 16);
    std::string hashPassword(const std::string& password, const std::string& salt, int iterations = 100000);
}