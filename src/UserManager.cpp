#include "UserManager.h"
#include "Logger.h"
#include "StatementWrapper.h"
#include "PasswordHasher.h"
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <stdexcept>

UserManager::UserManager(sqlite3* database) : db(database) {}

int UserManager::registerUser(const std::string& username, const std::string& password) {
    if (userExists(username)) {
        std::string errorMessage = "Registration error: user \"" + username + "\" already exists.";
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    if (username.empty() || password.empty()) {
        throw std::invalid_argument("Username and password cannot be empty");
    }

    std::string salt = PasswordHasher::generateSalt();
    std::string hashed = PasswordHasher::hashPassword(password, salt);

    const char* sql = "INSERT INTO users (username, password_hash, salt) VALUES (?, ?, ?);";

    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 2, hashed.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 3, salt.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc != SQLITE_DONE) {
        std::string errorMessage = std::string("Registration error: failed to insert user \"") + username + "\" " + sqlite3_errmsg(db);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    std::cout << "Successful registration" << std::endl;
    int userId = static_cast<int>(sqlite3_last_insert_rowid(db));
    Logger::getInstance().log(LogLevel::INFO, "User \"" + username + "\" successfully registered with ID " + std::to_string(userId));
    return userId;
}

int UserManager::loginUser(const std::string& username, const std::string& password) {
    const char* sql = "SELECT id, password_hash, salt FROM users WHERE username = ?;";

    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();

    if (rc == SQLITE_DONE) {
        std::cout << "Access denied" << std::endl;
        Logger::getInstance().log(LogLevel::WARNING, "Access denied for user \"" + username + "\". No such user.");
        return -1;
    }

    if (rc != SQLITE_ROW) {
        std::string errorMessage = std::string("Login error: query failed for user \"") + username + "\". " + sqlite3_errmsg(db);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    int userId = sqlite3_column_int(stmt.get(), 0);
    const unsigned char* storedHashText = sqlite3_column_text(stmt.get(), 1);
    const unsigned char* storedSaltText = sqlite3_column_text(stmt.get(), 2);

    if (!storedHashText || !storedSaltText) {
        std::cout << "Access denied" << std::endl;
        Logger::getInstance().log(LogLevel::WARNING, "Access denied for user \"" + username + "\". Invalid stored credentials.");
        return -1;
    }

    std::string storedHash(reinterpret_cast<const char*>(storedHashText));
    std::string storedSalt(reinterpret_cast<const char*>(storedSaltText));

    std::string hashedInput = PasswordHasher::hashPassword(password, storedSalt);

    if (hashedInput == storedHash) {
        std::cout << "Successful login" << std::endl;
        Logger::getInstance().log(LogLevel::INFO, "User \"" + username + "\" (ID: " + std::to_string(userId) + ") successfully logged in.");
        return userId;
    }
    else {
        std::cout << "Access denied" << std::endl;
        Logger::getInstance().log(LogLevel::WARNING, "Access denied for user \"" + username + "\". Incorrect password.");
        return -1;
    }
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

void UserManager::deleteUser(const std::string& username) {
    if (!userExists(username)) {
        std::string errorMessage = "Deletion error: user \"" + username + "\" doesn't exist.";
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    const char* sql = "DELETE FROM users WHERE username = ?;";

    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc != SQLITE_DONE) {
        std::string errorMessage = std::string("Deletion error: failed to delete user \"") + username + "\" " + sqlite3_errmsg(db);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    std::cout << "Successful deletion" << std::endl;
    Logger::getInstance().log(LogLevel::INFO, "User \"" + username + "\" successfully deleted.");
}

int UserManager::getUserId(const std::string& username) {
    const char* sql = "SELECT id FROM users WHERE username = ?;";

    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc == SQLITE_ROW) {
        return sqlite3_column_int(stmt.get(), 0);
    }
    else {
        std::string errorMessage = "User \"" + username + "\" not found.";
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }
}

std::string UserManager::getSaltByUsername(const std::string& username) {
    const char* sql = "SELECT salt FROM users WHERE username = ?;";
    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc == SQLITE_ROW) {
        const unsigned char* saltText = sqlite3_column_text(stmt.get(), 0);
        return std::string(reinterpret_cast<const char*>(saltText));
    } else {
        throw std::runtime_error("User not found when retrieving salt");
    }
}

std::pair<int, std::vector<unsigned char>> UserManager::loginAndDeriveKey(
    const std::string& username,
    const std::string& password)
{
    const char* sql = "SELECT id, password_hash, salt FROM users WHERE username = ?;";
    StatementWrapper stmt(db, sql);

    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc == SQLITE_ROW) {
        int userId = sqlite3_column_int(stmt.get(), 0);
        std::string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
        std::string salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));

        std::string inputHash = PasswordHasher::hashPassword(password, salt);

        if (inputHash == storedHash) {
            auto key = PasswordHasher::deriveKey(password, salt);
            return {userId, key};
        } else {
            return {-1, {}}; // Invalid password
        }
    } else {
        return {-1, {}}; // User not found
    }
}