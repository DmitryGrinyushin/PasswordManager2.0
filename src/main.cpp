#include "Logger.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <sqlite3.h>

namespace fs = std::filesystem;

bool fileExists(const std::string& filename) {
    return (fs::exists(filename));
}

int main() {
    try {
        Logger& logger = Logger::getInstance();
        logger.log(LogLevel::INFO, "Starting application");
    } catch (const std::exception& ex) {
        std::cerr << "Logfile initialization error: " << ex.what() << std::endl;
        return 1;
    }

    const std::string DB_PATH = "data/passdb.sqlite";

    bool dbExists = fileExists(DB_PATH);

    try {
        sqlite3* db;
        fs::create_directories("data");
        int rc = sqlite3_open(DB_PATH.c_str(), &db);

        if (rc != SQLITE_OK) {
            std::string errorMessage = std::string("Database opening error: ") + sqlite3_errmsg(db);
            Logger::getInstance().log(LogLevel::ERROR, errorMessage);
            sqlite3_close(db);
            throw std::runtime_error(errorMessage);
        }

        if (!dbExists) {
            const char* createUsersTable = 
                "CREATE TABLE users ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "username TEXT NOT NULL UNIQUE,"
                "password_hash TEXT NOT NULL);";

            const char* createPasswordsTable = 
                "CREATE TABLE passwords ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "value TEXT NOT NULL,"
                "type TEXT,"
                "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

            char* errMsg = nullptr;

            // Creating a table users
            rc = sqlite3_exec(db, createUsersTable, nullptr, nullptr, &errMsg);
            if (rc != SQLITE_OK) {
                std::string errorMessage = std::string("Error creating a table users: ") + errMsg;
                sqlite3_free(errMsg);
                Logger::getInstance().log(LogLevel::ERROR, errorMessage);
                sqlite3_close(db);
                throw std::runtime_error(errorMessage);
            }

            // Creating a table passwords
            rc = sqlite3_exec(db, createPasswordsTable, nullptr, nullptr, &errMsg);
            if (rc != SQLITE_OK) {
                std::string errorMessage = std::string("Error creating a table passwords: ") + errMsg;
                sqlite3_free(errMsg);
                Logger::getInstance().log(LogLevel::ERROR, errorMessage);
                sqlite3_close(db);
                throw std::runtime_error(errorMessage);
            }

            std::cout << "Database created with tables 'users' and 'passwords'." << std::endl;
            Logger::getInstance().log(LogLevel::INFO, "Database created");
        }
        else {
            std::cout << "Database file found. Connection established." << std::endl;
            Logger::getInstance().log(LogLevel::INFO, "Database connected");
        }

        sqlite3_close(db);

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}