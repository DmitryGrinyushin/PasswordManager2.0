#include "DatabaseManager.h"
#include "Logger.h"
#include <iostream>
#include <filesystem>
#include <sqlite3.h>
#include <stdexcept>

namespace fs = std::filesystem;

bool fileExists(const std::string& filename) {
    return (fs::exists(filename));
}

DatabaseManager::DatabaseManager(const std::string& path) : dbPath(path) {}

void DatabaseManager::initialize() {

    bool dbExists = fileExists(dbPath);

    fs::path parentDir = fs::path(dbPath).parent_path();
    if (!parentDir.empty()) {
        fs::create_directories(parentDir);
    }
    int rc = sqlite3_open(dbPath.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::string errorMessage = std::string("Database opening error: ") + sqlite3_errmsg(db);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        sqlite3_close(db);
        throw std::runtime_error(errorMessage);
    }

    if (!dbExists) {
        createTables();
    }
    else {
        std::cout << "Database file found. Connection established." << std::endl;
        Logger::getInstance().log(LogLevel::INFO, "Database connected");
    }
}

void DatabaseManager::createTables() {

    const char* createUsersTable = 
        "CREATE TABLE users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password_hash TEXT NOT NULL,"
        "salt TEXT NOT NULL);";

    const char* createPasswordsTable = 
        "CREATE TABLE passwords ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "value TEXT NOT NULL,"
        "type TEXT,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    char* errMsg = nullptr;

    // Creating a table users
    int rc = sqlite3_exec(db, createUsersTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string errorMessage = std::string("Error creating a table users: ") + errMsg;
        sqlite3_free(errMsg);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    // Creating a table passwords
    rc = sqlite3_exec(db, createPasswordsTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string errorMessage = std::string("Error creating a table passwords: ") + errMsg;
        sqlite3_free(errMsg);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    std::cout << "Database created with tables 'users' and 'passwords'." << std::endl;
    Logger::getInstance().log(LogLevel::INFO, "Database created");

}

sqlite3* DatabaseManager::getDb() {
    return db;
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}