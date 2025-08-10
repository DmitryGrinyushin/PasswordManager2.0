#include "WebServer.h"
#include "json.hpp"
#include "DatabaseManager.h"
#include "EnvUtils.h"
#include "UserManager.h"
#include <iostream>

int main() {
    
    std::string host = "0.0.0.0";
    int port = 8080;

    std::string dbPath = getEnvOrDefault("PASSWORD_MANAGER_DB_PATH", "data/passdb.sqlite");
    DatabaseManager dbManager(dbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());
    WebServer server(host, port, userManager);

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