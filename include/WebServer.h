#pragma once

#include <httplib.h>
#include <string>

class WebServer {
    httplib::Server server_;
    int port_;
    std::string host_;

    void setupRoutes();

public:
    WebServer(std::string& host, int port);
    void start();
    void stop();
};