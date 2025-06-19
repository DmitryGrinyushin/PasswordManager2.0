#include <iostream>
#include <string>
#include <filesystem>
#include <sqlite3.h>

namespace fs = std::filesystem;

bool fileExists(const std::string& filename) {
    return (fs::exists(filename));
}

int main() {

    const std::string DB_PATH = "data/passdb.sqlite";

    bool dbExists = fileExists(DB_PATH);

    sqlite3* db;
    int rc = sqlite3_open(DB_PATH.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Database opening error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
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
            std::cerr << "Error creating a table users: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            sqlite3_close(db);
            return 1;
        }

        // Creating a table passwords
        rc = sqlite3_exec(db, createPasswordsTable, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Error creating a table passwords: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            sqlite3_close(db);
            return 1;
        }
    
        std::cout << "The database was created with tables users and passwords." << std::endl;
    }
    else {
        std::cout << "The database was found and connected." << std::endl;
    }
    
    sqlite3_close(db);
    return 0;
    
}