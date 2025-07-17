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

    SECTION("Account is added") {
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

    SECTION("Account is updated") {
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

    SECTION("Account is deleted") {
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

    SECTION("Multiple accounts are added") {
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
    
    SECTION("Multiple accounts for one user are available") {
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
}