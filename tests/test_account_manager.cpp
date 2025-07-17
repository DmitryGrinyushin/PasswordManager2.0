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
    int userId = userManager.registerUser("testuser", "testpass");
    userManager.loginUser("testuser", "testpass");

    AccountManager accountManager(dbManager.getDb());

    SECTION("Add account") {
        accountManager.addAccount(userId,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts.size() == 1);
        REQUIRE(accounts[0].accountName == "testaccount");
        REQUIRE(accounts[0].login == "testlogin");
        REQUIRE(accounts[0].passwordHash == "testpasswordHash");
        REQUIRE(accounts[0].notes == "testnotes");
    }

    SECTION("Update account") {
        accountManager.addAccount(userId,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.updateAccount(accountId,
                          "testaccount_1",
                          "testlogin_1",
                          "testpasswordHash_1",
                          "testnotes_1");

        auto accounts_1 = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts_1.size() == 1);
        REQUIRE(accounts_1[0].accountName == "testaccount_1");
        REQUIRE(accounts_1[0].login == "testlogin_1");
        REQUIRE(accounts_1[0].passwordHash == "testpasswordHash_1");
        REQUIRE(accounts_1[0].notes == "testnotes_1");
    }

    SECTION("Delete account") {
        accountManager.addAccount(userId,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.deleteAccount(accountId);
        auto accounts_1 = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts_1.size() == 0);
    }

    SECTION("Add accounts for multiple users") {
        int userId_2 = userManager.registerUser("seconduser", "secondpass");
        userManager.loginUser("seconduser", "secondpass");

        accountManager.addAccount(userId,
                          "testaccount_1",
                          "testlogin_1",
                          "testpasswordHash_1",
                          "testnotes_1");

        accountManager.addAccount(userId_2,
                          "testaccount_2",
                          "testlogin_2",
                          "testpasswordHash_2",
                          "testnotes_2");

        auto accounts_1 = accountManager.getAccountsForUser(userId);
        auto accounts_2 = accountManager.getAccountsForUser(userId_2);

        REQUIRE(accounts_1.size() == 1);
        REQUIRE(accounts_2.size() == 1);

        REQUIRE(accounts_1[0].accountName == "testaccount_1");
        REQUIRE(accounts_1[0].login == "testlogin_1");
        REQUIRE(accounts_1[0].passwordHash == "testpasswordHash_1");
        REQUIRE(accounts_1[0].notes == "testnotes_1");

        REQUIRE(accounts_2[0].accountName == "testaccount_2");
        REQUIRE(accounts_2[0].login == "testlogin_2");
        REQUIRE(accounts_2[0].passwordHash == "testpasswordHash_2");
        REQUIRE(accounts_2[0].notes == "testnotes_2");
    }
    
    SECTION("Add multiple accounts for a single user") {
        accountManager.addAccount(userId,
                          "testaccount_1",
                          "testlogin_1",
                          "testpasswordHash_1",
                          "testnotes_1");

        accountManager.addAccount(userId,
                          "testaccount_2",
                          "testlogin_2",
                          "testpasswordHash_2",
                          "testnotes_2");

        auto accounts = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts.size() == 2);

        REQUIRE(accounts[0].accountName == "testaccount_1");
        REQUIRE(accounts[0].login == "testlogin_1");
        REQUIRE(accounts[0].passwordHash == "testpasswordHash_1");
        REQUIRE(accounts[0].notes == "testnotes_1");

        REQUIRE(accounts[1].accountName == "testaccount_2");
        REQUIRE(accounts[1].login == "testlogin_2");
        REQUIRE(accounts[1].passwordHash == "testpasswordHash_2");
        REQUIRE(accounts[1].notes == "testnotes_2");
    }

    SECTION("Update non-existent account") {
        accountManager.addAccount(userId,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.deleteAccount(accountId);
        auto accounts_1 = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts_1.size() == 0);

        REQUIRE_THROWS_AS(accountManager.updateAccount(accountId,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes"), std::runtime_error);
    }

    SECTION("Delete non-existent account") {
        accountManager.addAccount(userId,
                          "testaccount",
                          "testlogin",
                          "testpasswordHash",
                          "testnotes");

        auto accounts = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.deleteAccount(accountId);
        auto accounts_1 = accountManager.getAccountsForUser(userId);
        REQUIRE(accounts_1.size() == 0);
        
        REQUIRE_THROWS_AS(accountManager.deleteAccount(accountId), std::runtime_error);
    }

    SECTION("Get accounts for non-existent user") {
        auto accounts = accountManager.getAccountsForUser(99999);
        REQUIRE(accounts.empty());
    }
}