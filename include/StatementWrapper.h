#pragma once

#include <string>
#include <sqlite3.h>

class StatementWrapper {
    sqlite3_stmt* stmt = nullptr;

public:
    StatementWrapper (sqlite3* db, const char* sql);
    ~StatementWrapper ();

    sqlite3_stmt* get();
    int step();
};