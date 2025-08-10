#include "WebServer.h"
#include "json.hpp"
#include <iostream>

WebServer::WebServer(std::string& host, int port, UserManager& userManager)
                : host_(host), port_(port), userManager_(userManager) {}

void WebServer::setupRoutes() {
    server_.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Password Manager Web API is running!", "text/plain");
    });

    server_.Post("/register", [this] (const httplib::Request& req, httplib::Response& res) {
        try {
            auto jsonBody = nlohmann::json::parse(req.body);

            if (!jsonBody.contains("username") || !jsonBody.contains("password")) {
                res.status = 400; // Bad Request
                res.set_content(R"({"error": "Missing username or password"})", "application/json");
                return;
            }

            std::string username = jsonBody["username"];
            std::string password = jsonBody["password"];

            try {
                userManager_.registerUser(username, password);
                res.status = 201; // Created
                res.set_content(R"({"message": "User registered successfully"})", "application/json");
            } catch (const std::exception& e) {
                res.status = 409; // Conflict (user already exists)
                std::string errorJson = std::string(R"({"error": ")") + e.what() + "\"}";
                res.set_content(errorJson, "application/json");
            }
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(R"({"error": "Invalid JSON"})", "application/json");
            return;
        }
    });

    // server_.Post("/login", ...);
}

void WebServer::start() {
    setupRoutes();
    std::cout << "Server starting at " << host_ << ":" << port_ << std::endl;
    server_.listen(host_.c_str(), port_);
}

void WebServer::stop() {
    server_.stop();
}