#pragma once

#include "UserManager.h"
#include "JWTManager.h"
#include <httplib.h>
#include <string>

class WebServer {
    httplib::Server server_;
    int port_;
    std::string host_;
    UserManager& userManager_;
    JWTManager jwtManager_;

    void setupRoutes();

public:
    WebServer(std::string& host, int port, UserManager& userManager);
    WebServer(UserManager& userManager, const std::string& jwtSecret);
    void start();
    void stop();
};