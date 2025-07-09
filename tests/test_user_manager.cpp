#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "UserManager.h"
#include "DatabaseManager.h"
#include "Logger.h"

TEST_CASE("UserManager basic operations", "[user]") {
    // preparation
    std::string testDbPath = ":memory:";
    DatabaseManager dbManager(testDbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());

    SECTION("User registration") {
        REQUIRE(userManager.userExists("testuser") == false);
        REQUIRE_NOTHROW(userManager.registerUser("testuser", "pass123"));
        REQUIRE(userManager.userExists("testuser") == true);
    }

    SECTION("User login") {
        userManager.registerUser("testuser", "pass123");
        REQUIRE(userManager.userExists("testuser") == true);
        REQUIRE(userManager.loginUser("testuser", "pass123") == true);
        REQUIRE(userManager.loginUser("testuser", "wrongpass") == false);
    }

    SECTION("User deletion") {
        userManager.registerUser("testuser", "pass123");
        REQUIRE(userManager.userExists("testuser") == true);
        REQUIRE_NOTHROW(userManager.deleteUser("testuser"));
        REQUIRE(userManager.userExists("testuser") == false);
    }

    SECTION("Duplicate registration throws exception") {
        REQUIRE_NOTHROW(userManager.registerUser("testuser", "pass123"));
        REQUIRE_THROWS_AS(userManager.registerUser("testuser", "anotherpass"), std::runtime_error);
    }

    SECTION("Deleting non-existent user throws exception") {
        REQUIRE(userManager.userExists("ghostuser") == false);
        REQUIRE_THROWS_AS(userManager.deleteUser("ghostuser"), std::runtime_error);
    }

    SECTION("Login of non-existent user fails") {
        REQUIRE(userManager.userExists("ghostuser") == false);
        REQUIRE(userManager.loginUser("ghostuser", "whatever") == false);
    }

    SECTION("Deleted user disappearance") {
        userManager.registerUser("testuser", "pass123");
        userManager.deleteUser("testuser");
        REQUIRE(userManager.userExists("testuser") == false);
        REQUIRE(userManager.loginUser("testuser", "wrongpass") == false);
    }

    SECTION("Multiple user login") {
        userManager.registerUser("testuser1", "pass123");
        REQUIRE(userManager.userExists("testuser1") == true);
        REQUIRE(userManager.loginUser("testuser1", "pass123") == true);
        userManager.registerUser("testuser2", "pass456");
        REQUIRE(userManager.userExists("testuser2") == true);
        REQUIRE(userManager.loginUser("testuser2", "pass456") == true);
        REQUIRE(userManager.loginUser("testuser1", "pass456") == false);
        REQUIRE(userManager.loginUser("testuser2", "pass123") == false);
    }
}