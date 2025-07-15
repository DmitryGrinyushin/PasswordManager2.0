#include "PasswordHasher.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>

namespace PasswordHasher {

std::string generateSalt(size_t length) {
    std::string salt(length, '\0');
    RAND_bytes(reinterpret_cast<unsigned char*>(&salt[0]), length);
    std::ostringstream oss;
    for (unsigned char c : salt) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return oss.str();
}

std::string hashPassword(const std::string& password, const std::string& salt, int iterations) {
    unsigned char hash[32]; // 256-bit
    std::string combined = salt + password;

    PKCS5_PBKDF2_HMAC(
        password.c_str(),
        password.size(),
        reinterpret_cast<const unsigned char*>(salt.c_str()),
        salt.size(),
        iterations,
        EVP_sha256(),
        sizeof(hash),
        hash
    );

    std::ostringstream oss;
    for (unsigned char c : hash) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return oss.str();
}

}