#pragma once

#include <iostream>
#include <sqlite3.h>

class DatabaseManager {
    sqlite3* db = nullptr;
    std::string dbPath;

    void executeSQL(const char* tableSQL, const std::string& tableName);
    void createTables();

public:
    DatabaseManager(const std::string& path);
    
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    DatabaseManager(DatabaseManager&&) = delete;
    DatabaseManager& operator=(DatabaseManager&&) = delete;

    void initialize();

    sqlite3* getDb();

    ~DatabaseManager();
};