#pragma once

#include "UserManager.h"
#include <httplib.h>
#include <string>

class WebServer {
    httplib::Server server_;
    int port_;
    std::string host_;
    UserManager& userManager_;

    void setupRoutes();

public:
    WebServer(std::string& host, int port, UserManager& userManager);
    void start();
    void stop();
};