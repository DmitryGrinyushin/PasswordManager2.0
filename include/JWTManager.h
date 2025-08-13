#pragma once

#include <string>
#include <chrono>
#include <jwt-cpp/jwt.h>

class JWTManager {
    static std::string secretKey;

public:
    static void initSecret();
    // token generation with time_to_live sec
    static std::string generateToken(const std::string& username, int ttlSeconds = 3600);

    // token verification and username extraction
    // throws exception!!!
    static std::string verifyToken(const std::string& token);
};