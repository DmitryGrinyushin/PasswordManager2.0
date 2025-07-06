#include "UserManager.h"
#include "Logger.h"
#include "StatementWrapper.h"
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <stdexcept>

UserManager::UserManager(sqlite3* database) : db(database) {}

void UserManager::registerUser(const std::string& username, const std::string& password) {
    if (userExists(username)) {
        std::string errorMessage = "Registration error: user \"" + username + "\" already exists.";
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    const char* sql = "INSERT INTO users (username, password_hash) VALUES (?, ?);";
    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 2, password.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc != SQLITE_DONE) {
        std::string errorMessage = std::string("Error inserting user: ") + sqlite3_errmsg(db);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    Logger::getInstance().log(LogLevel::INFO, "User \"" + username + "\" successfully registered.");
}

bool UserManager::loginUser(const std::string& username, const std::string& password) {

}

bool UserManager::userExists(const std::string& username) {
    const char* sql = "SELECT COUNT(*) FROM users WHERE username = ?;";

    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc != SQLITE_ROW) {
        std::string errorMessage = std::string("Database request execution error: ") + sqlite3_errmsg(db);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    int count = sqlite3_column_int(stmt.get(), 0);

    return (count > 0);
}