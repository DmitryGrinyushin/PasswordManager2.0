#pragma once
#include <string>

namespace PasswordHasher {
    std::string generateSalt(size_t length = 16);
    std::string hashPassword(const std::string& password, const std::string& salt, int iterations = 100000);

    std::vector<unsigned char> deriveKey(const std::string& password, const std::string& salt, int keyLength = 32);
}