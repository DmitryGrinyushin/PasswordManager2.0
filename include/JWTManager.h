#pragma once

#include <string>
#include <chrono>
#include <utility>

class JWTManager {
    static std::string secretKey;

public:
    JWTManager() = default;
    explicit JWTManager(const std::string& secret) { secretKey = secret; }

    static void initSecret();
    // token generation with time_to_live sec
    std::string generateToken(int userId, const std::string username, int ttlSeconds = 3600);

    // token verification and username extraction
    // throws exception!!!
    std::pair<int, std::string> verifyToken(const std::string& token) const;
};