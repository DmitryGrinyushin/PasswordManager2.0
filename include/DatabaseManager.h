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
    void initialize();

    sqlite3* getDb();

    ~DatabaseManager();
};