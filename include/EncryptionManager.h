#pragma once

#include <string>
#include <vector>

class EncryptionManager {
public:
    static std::vector<unsigned char> generateRandomBytes(size_t length);

    static std::vector<unsigned char> encrypt(
        const std::string& plaintext,
        const std::vector<unsigned char>& key,
        std::vector<unsigned char>& outIV,
        std::vector<unsigned char>& outTag
    );

    static std::string decrypt(
        const std::vector<unsigned char>& ciphertext,
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& iv,
        const std::vector<unsigned char>& tag
    );

    static std::string encryptField(const std::string& plaintext, const std::vector<unsigned char>& key);
    static std::string decryptField(const std::string& encrypted, const std::vector<unsigned char>& key);
};