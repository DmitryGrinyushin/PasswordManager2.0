#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "DatabaseManager.h"
#include "Logger.h"

TEST_CASE("Database creating in memory", "[db][memory]") {
    std::string dbPath = ":memory:";
    DatabaseManager dbManager(dbPath);
    dbManager.initialize();
    sqlite3* db = dbManager.getDb();

    SECTION("Users table exists") {
        const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='users';";
        sqlite3_stmt* stmt;
        REQUIRE(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK);
        REQUIRE(sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
    }

    SECTION("Accounts table exists") {
        const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='accounts';";
        sqlite3_stmt* stmt;
        REQUIRE(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK);
        REQUIRE(sqlite3_step(stmt) == SQLITE_ROW);
        sqlite3_finalize(stmt);
    }
}

TEST_CASE("Database creating in directory", "[db][file]") {
    std::string dbPath = "tests/test_db.sqlite";

    SECTION("Multiple database initialization does not throw") {
        DatabaseManager dbManager(dbPath);
        dbManager.initialize();
        REQUIRE(std::filesystem::exists(dbPath) == true);
        REQUIRE_NOTHROW(dbManager.initialize());
    }
    std::remove("tests/test_db.sqlite");
}