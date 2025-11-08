#include "JWTManager.h"
#include <cstdlib>     // getenv
#include <random>
#include <jwt-cpp/jwt.h>

std::string JWTManager::secretKey;

JWTManager::JWTManager(const std::string& secret) {
    if (secret.empty()) {
        throw std::runtime_error("JWT secret cannot be empty");
    }
    secretKey = secret;
}

std::string JWTManager::generateToken(int userId, std::string username, int ttlSeconds) {
    auto now = std::chrono::system_clock::now();
    auto exp = now + std::chrono::seconds{ttlSeconds};
    auto token = jwt::create()
        .set_issuer("PasswordManager2.0")
        .set_payload_claim("userId", jwt::claim(std::to_string(userId)))
        .set_payload_claim("username", jwt::claim(username))
        .set_issued_at(now)
        .set_expires_at(exp)
        .sign(jwt::algorithm::hs256{ secretKey });
    return token;
}

std::pair<int, std::string> JWTManager::verifyToken(const std::string& token) const {
    auto decoded = jwt::decode(token);
    auto verifier = jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{ secretKey })
        .with_issuer("PasswordManager2.0");

    verifier.verify(decoded);

    int userId = std::stoi(decoded.get_payload_claim("userId").as_string());
    std::string username = decoded.get_payload_claim("username").as_string();

    return {userId, username};
}