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

void DatabaseManager::executeSQL(const char* tableSQL, const std::string& tableName) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, tableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string errorMessage = std::string("Error creating table ") + tableName + ": " + errMsg;
        sqlite3_free(errMsg);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }
}

void DatabaseManager::createTables() {

    const char* createUsersTable = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password_hash TEXT NOT NULL,"
        "salt TEXT NOT NULL);";

    const char* createAccountsTable =
        "CREATE TABLE IF NOT EXISTS accounts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "account_name TEXT NOT NULL,"
        "login TEXT,"
        "password_hash TEXT,"
        "url TEXT,"
        "notes TEXT,"
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE);";

    executeSQL(createUsersTable, "users");
    executeSQL(createAccountsTable, "accounts");

    std::cout << "Database created with tables 'users', 'accounts'." << std::endl;
    Logger::getInstance().log(LogLevel::INFO, "Database created");
}

sqlite3* DatabaseManager::getDb() {
    return db;
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}