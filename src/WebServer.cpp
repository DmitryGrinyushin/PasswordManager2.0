#include "WebServer.h"
#include "json.hpp"
#include <iostream>

WebServer::WebServer(std::string& host, int port, UserManager& userManager)
                : host_(host), port_(port), userManager_(userManager) {}

WebServer::WebServer(UserManager& userManager, const std::string& jwtSecret) 
                : userManager_(userManager), jwtManager_(jwtSecret) {}

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

    server_.Post("/login", [this] (const httplib::Request& req, httplib::Response& res) {
        try {
            auto jsonBody = nlohmann::json::parse(req.body);

            if (!jsonBody.contains("username") || !jsonBody.contains("password")) {
                res.status = 400; // Bad Request
                res.set_content(R"({"error": "Missing username or password"})", "application/json");
                return;
            }

            std::string username = jsonBody["username"];
            std::string password = jsonBody["password"];

            int userId = userManager_.loginUser(username, password);

            if (userId == -1) {
                res.status = 401; // Unauthorized
                res.set_content(R"({"error": "Invalid username or password"})", "application/json");
                return;
            }

        // JWT generation
        std::string token = jwtManager_.generateToken(userId, username, 3600);

        // responce with token
        nlohmann::json responseJson;
        responseJson["message"] = "User logged in successfully";
        responseJson["token"] = token;

            res.status = 200; // OK
            res.set_content(responseJson.dump(), "application/json");

        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(R"({"error": "Invalid JSON"})", "application/json");
            return;
        }
    });
}

std::optional<std::pair<int, std::string>> WebServer::authenticateRequest(const httplib::Request& req, httplib::Response& res) {
    if (!req.has_header("Authorization")) {
        res.status = 401;
        res.set_content(R"({"error": "Missing Authorization header"})", "application/json");
        return std::nullopt;
    }

    std::string authHeader = req.get_header_value("Authorization");
    if (authHeader.rfind("Bearer ", 0) != 0) {  // проверяем, что начинается с Bearer
        res.status = 401;
        res.set_content(R"({"error": "Invalid Authorization format"})", "application/json");
        return std::nullopt;
    }

    std::string token = authHeader.substr(7); // убираем "Bearer "

    try {
        auto [userId, username] = jwtManager_.verifyToken(token);
        return std::make_pair(userId, username);
    } catch (const std::exception& e) {
        res.status = 401;
        res.set_content(R"({"error": "Invalid or expired token"})", "application/json");
        return std::nullopt;
    }
}

void WebServer::start() {
    setupRoutes();
    std::cout << "Server starting at " << host_ << ":" << port_ << std::endl;
    server_.listen(host_.c_str(), port_);
}

void WebServer::stop() {
    server_.stop();
}