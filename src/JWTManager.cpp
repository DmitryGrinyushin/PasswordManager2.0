#include "JWTManager.h"
#include <cstdlib>     // getenv
#include <random>
#include <jwt-cpp/jwt.h>

std::string JWTManager::secretKey;

void JWTManager::initSecret() {
    const char* envSecret = std::getenv("JWT_SECRET");
    if (envSecret) {
        secretKey = envSecret;
    } else {
        // fallback: random secret generation (for the server session duration)
        static const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
        
        secretKey.resize(32);
        for (auto& ch : secretKey) {
            ch = charset[dist(gen)];
        }
    }
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