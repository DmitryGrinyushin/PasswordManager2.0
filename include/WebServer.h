#pragma once

#include "UserManager.h"
#include "JWTManager.h"
#include "AccountManager.h"
#include <httplib.h>
#include <string>
#include <optional>
#include <unordered_map>

class WebServer {
    httplib::Server server_;
    int port_;
    std::string host_;
    UserManager& userManager_;
    JWTManager jwtManager_;
    AccountManager& accountManager_;
    std::unordered_map<int, std::vector<unsigned char>> activeKeys_;

    void setupRoutes();
    std::optional<std::pair<int, std::string>> authenticateRequest(const httplib::Request& req, httplib::Response& res);

public:
    WebServer(std::string& host, int port, UserManager& userManager, AccountManager& accountManager, const std::string& jwtSecret);
    
    void start();
    void stop();
};