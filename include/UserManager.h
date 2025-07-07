#pragma once

#include <string>
#include <sqlite3.h>

class UserManager {
    sqlite3* db;

public:
    UserManager(sqlite3* database);

    void registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    bool userExists(const std::string& username);
    void deleteUser(const std::string& username);
};