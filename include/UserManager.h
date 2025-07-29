#pragma once

#include <string>
#include <sqlite3.h>

class UserManager {
    sqlite3* db;

public:
    UserManager(sqlite3* database);

    int registerUser(const std::string& username, const std::string& password);
    int loginUser(const std::string& username, const std::string& password);
    bool userExists(const std::string& username);
    void deleteUser(const std::string& username);

    int getUserId(const std::string& username);
    std::string getSaltByUsername(const std::string& username);

    std::pair<int, std::vector<unsigned char>> loginAndDeriveKey(
    const std::string& username,
    const std::string& password);
};