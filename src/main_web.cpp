#include "WebServer.h"
#include "json.hpp"
#include "DatabaseManager.h"
#include "EnvUtils.h"
#include "UserManager.h"
#include <iostream>
#include <jwt-cpp/jwt.h>

// JWT test
void testJWTManager() {
    JWTManager jwtManager;
    jwtManager.initSecret();  // inicializing the secret

    int userId = 42;
    std::string username = "testuser";

    // create token
    auto token = jwtManager.generateToken(userId, username, 3600);
    std::cout << "JWT token: " << token << std::endl;

    // token verification
    auto [decodedUserId, decodedUsername] = jwtManager.verifyToken(token);

    if (!decodedUsername.empty()) {
        std::cout << "Decoded userId: " << decodedUserId << std::endl;
        std::cout << "Decoded username: " << decodedUsername << std::endl;
    } else {
        std::cout << "Invalid token" << std::endl;
    }
}
// ======

int main() {
    
    std::string host = "0.0.0.0";
    int port = 8080;

    std::string dbPath = getEnvOrDefault("PASSWORD_MANAGER_DB_PATH", "data/passdb.sqlite");
    DatabaseManager dbManager(dbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());
    WebServer server(host, port, userManager);

// JWT test
    std::cout << "Test JWTManager..." << std::endl;
    testJWTManager();
//========

    std::thread serverThread([&server]() {
        server.start();
    });

    std::cout << "Server started. Press Enter to stop...\n";
    std::cin.get();

    std::cout << "Stopping server...\n";
    server.stop();

    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "Server stopped.\n";

    return 0;
}