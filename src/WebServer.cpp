#include "WebServer.h"
#include <iostream>

WebServer::WebServer(std::string& host, int port) : host_(host), port_(port) {}

void WebServer::setupRoutes() {
    server_.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Password Manager Web API is running!", "text/plain");
    });

    // server_.Post("/login", ...);
    // server_.Post("/register", ...);
}

void WebServer::start() {
    setupRoutes();
    std::cout << "Server starting at " << host_ << ":" << port_ << std::endl;
    server_.listen(host_.c_str(), port_);
}

void WebServer::stop() {
    server_.stop();
}