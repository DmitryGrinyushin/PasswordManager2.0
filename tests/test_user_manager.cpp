#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "UserManager.h"
#include "DatabaseManager.h"
#include "Logger.h"

TEST_CASE("UserManager basic operations", "[user]") {
    // preparation
    std::string testDbPath = "tests/test_db.sqlite";
    DatabaseManager dbManager(testDbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());

    SECTION("User registration") {
        REQUIRE_NOTHROW(userManager.registerUser("testuser", "pass123"));
        REQUIRE(userManager.userExists("testuser") == true);
    }

    SECTION("User login") {
        REQUIRE(userManager.loginUser("testuser", "pass123") == true);
        REQUIRE(userManager.loginUser("testuser", "wrongpass") == false);
    }

    SECTION("User deletion") {
        REQUIRE_NOTHROW(userManager.deleteUser("testuser"));
        REQUIRE(userManager.userExists("testuser") == false);
    }
}