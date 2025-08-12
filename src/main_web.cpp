#include "WebServer.h"
#include "json.hpp"
#include "DatabaseManager.h"
#include "EnvUtils.h"
#include "UserManager.h"
#include <iostream>
#include <jwt-cpp/jwt.h>

void testJWT() {
    // секрет, в реале нужно брать из env переменной
    std::string secret = "supersecret";

    // создаём токен с payload username = "testuser"
    auto token = jwt::create()
        .set_issuer("PasswordManager2.0")
        .set_payload_claim("username", jwt::claim(std::string("testuser")))
        .sign(jwt::algorithm::hs256{secret});

    std::cout << "JWT token: " << token << std::endl;

    // Декодируем и проверяем
    auto decoded = jwt::decode(token);

    std::cout << "Decoded username: " << decoded.get_payload_claim("username").as_string() << std::endl;
}

int main() {
    
    std::string host = "0.0.0.0";
    int port = 8080;

    std::string dbPath = getEnvOrDefault("PASSWORD_MANAGER_DB_PATH", "data/passdb.sqlite");
    DatabaseManager dbManager(dbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());
    WebServer server(host, port, userManager);

    //JWT test
    std::cout << "Запуск теста JWT..." << std::endl;
    testJWT();
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