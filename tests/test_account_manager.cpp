#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "UserManager.h"
#include "AccountManager.h"
#include "DatabaseManager.h"
#include "Logger.h"

TEST_CASE("AccountManager basic operations", "[account]") {
    std::string testDbPath = ":memory:"; // Database in RAM - recreated every SECTION request.
    DatabaseManager dbManager(testDbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());
    userManager.registerUser("testuser", "testpass");
    userManager.loginUser("testuser", "testpass");

    AccountManager accountManager(dbManager.getDb());

    SECTION("Account is added") {
        accountManager.addAccount(1,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(1);
        REQUIRE(accounts.size() == 1);
        REQUIRE(accounts[0].accountName == "testaccount");
        REQUIRE(accounts[0].login == "testlogin");
        REQUIRE(accounts[0].passwordHash == "testpasswordHash");
        REQUIRE(accounts[0].notes == "testnotes");
    }

    SECTION("Account is updated") {
        accountManager.addAccount(1,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(1);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.updateAccount(accountId,
                          "testaccount_1",
                          "testlogin_1",
                          "testpasswordHash_1",
                          "testnotes_1");

        auto accounts_1 = accountManager.getAccountsForUser(1);
        REQUIRE(accounts_1.size() == 1);
        REQUIRE(accounts_1[0].accountName == "testaccount_1");
        REQUIRE(accounts_1[0].login == "testlogin_1");
        REQUIRE(accounts_1[0].passwordHash == "testpasswordHash_1");
        REQUIRE(accounts_1[0].notes == "testnotes_1");
    }

    SECTION("Account is deleted") {
        accountManager.addAccount(1,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(1);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.deleteAccount(accountId);
        auto accounts_1 = accountManager.getAccountsForUser(1);
        REQUIRE(accounts_1.size() == 0);
    }
}