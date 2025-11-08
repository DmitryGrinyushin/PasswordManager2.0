#include "WebServer.h"
#include "json.hpp"
#include "DatabaseManager.h"
#include "EnvUtils.h"
#include "UserManager.h"
#include "AccountManager.h"
#include <iostream>
#include <thread>

// JWT test helper function to check token creation and verification
/*
void testJWTManager(const std::string& secret) {
    JWTManager jwtManager(secret);

    int userId = 42;
    std::string username = "testuser";

    // Generate a JWT token
    auto token = jwtManager.generateToken(userId, username, 3600);
    std::cout << "JWT token: " << token << std::endl;

    // Verify the generated token and print decoded information
    try {
        auto [decodedUserId, decodedUsername] = jwtManager.verifyToken(token);

        std::cout << "Decoded userId: " << decodedUserId << std::endl;
        std::cout << "Decoded username: " << decodedUsername << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Invalid token: " << e.what() << std::endl;
    }
}
*/
    std::string getEnvOrFail(const char* name) {
        auto v = std::getenv(name);
        if (!v || std::string(v).empty()) {
            std::cerr << "Fatal error: missing required ENV " << name << "\n";
            exit(1);
        }
        return v;
    }

int main() {
    // Server host and port settings
    std::string host = "0.0.0.0";
    int port = 8080;

    // Get database path from environment variable or use default
    std::string dbPath = getEnvOrDefault("PASSWORD_MANAGER_DB_PATH", "data/passdb.sqlite");

    // Initialize database manager and create/open database
    DatabaseManager dbManager(dbPath);
    dbManager.initialize();

    // Initialize UserManager and AccountManager with database handle
    UserManager userManager(dbManager.getDb());
    AccountManager accountManager(dbManager, userManager);

    // Get JWT secret key from environment variable or fail
    std::string jwtSecret = getEnvOrFail("JWT_SECRET");

    // Run a simple JWT test to ensure token generation and verification works
    /*
    std::cout << "Test JWTManager..." << std::endl;
    testJWTManager(jwtSecret);
    */

    // Create the WebServer instance with all managers and secret
    WebServer server(host, port, userManager, accountManager, jwtSecret);

    // Start the server in a separate thread
    std::thread serverThread([&server]() {
        server.start();
    });

    std::cout << "Server started. Press Enter to stop...\n";
    std::cin.get();

    // Stop the server gracefully on user input
    std::cout << "Stopping server...\n";
    server.stop();

    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "Server stopped.\n";

    return 0;
}
