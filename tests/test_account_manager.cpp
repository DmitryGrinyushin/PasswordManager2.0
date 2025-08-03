#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "UserManager.h"
#include "AccountManager.h"
#include "DatabaseManager.h"
#include "Logger.h"
#include "EncryptionManager.h"

TEST_CASE("AccountManager basic operations", "[account]") {
    std::string testDbPath = ":memory:"; // Database in RAM - recreated every SECTION request.
    DatabaseManager dbManager(testDbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());
    int userId = userManager.registerUser("testuser", "testpass");
    
    AccountManager accountManager(dbManager, userManager);
    const std::vector<unsigned char> dummyKey(32, 0x00);
    
    SECTION("Add account") {    
        accountManager.addAccount(userId,
                        "testaccount",
                        "testlogin",
                        "testpassword",
                        "testurl",
                        "testnotes",
                        dummyKey);

        auto accounts = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts.size() == 1);
        REQUIRE(accounts[0].accountName == "testaccount");
        REQUIRE(accounts[0].login == "testlogin");
        REQUIRE_FALSE(accounts[0].encryptedPassword.empty());
        REQUIRE(accounts[0].url == "testurl");
        REQUIRE(accounts[0].notes == "testnotes");
    }

    SECTION("Update account") {
        accountManager.addAccount(userId,
                        "testaccount",
                        "testlogin",
                        "testpassword",
                        "testurl",
                        "testnotes",
                        dummyKey);

        auto accounts = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.updateAccount(userId,
                        accountId,
                        "testaccount_1",
                        "testlogin_1",
                        "testpassword_1",
                        "testurl_1",
                        "testnotes_1",
                        dummyKey);

        auto accounts_1 = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts_1.size() == 1);
        REQUIRE(accounts_1[0].accountName == "testaccount_1");
        REQUIRE(accounts_1[0].login == "testlogin_1");
        REQUIRE_FALSE(accounts_1[0].encryptedPassword.empty());
        REQUIRE(accounts_1[0].url == "testurl_1");
        REQUIRE(accounts_1[0].notes == "testnotes_1");
    }

    SECTION("Delete account") {
        accountManager.addAccount(userId,
                        "testaccount",
                        "testlogin",
                        "testpassword",
                        "testurl",
                        "testnotes",
                        dummyKey);

        auto accounts = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.deleteAccount(userId, accountId);
        auto accounts_1 = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts_1.empty());
    }

    SECTION("Add accounts for multiple users") {
        int userId_2 = userManager.registerUser("seconduser", "secondpass");
        userManager.loginUser("seconduser", "secondpass");

        accountManager.addAccount(userId,
                        "testaccount_1",
                        "testlogin_1",
                        "testpassword_1",
                        "testurl_1",
                        "testnotes_1",
                        dummyKey);

        const std::vector<unsigned char> dummyKey_2(32, 0x00);
        accountManager.addAccount(userId_2,
                        "testaccount_2",
                        "testlogin_2",
                        "testpassword_2",
                        "testurl_2",
                        "testnotes_2",
                        dummyKey_2);

        auto accounts_1 = accountManager.getAccountsForUser(userId, dummyKey);
        auto accounts_2 = accountManager.getAccountsForUser(userId_2, dummyKey_2);

        REQUIRE(accounts_1.size() == 1);
        REQUIRE(accounts_2.size() == 1);

        REQUIRE(accounts_1[0].accountName == "testaccount_1");
        REQUIRE(accounts_1[0].login == "testlogin_1");
        REQUIRE_FALSE(accounts_1[0].encryptedPassword.empty());
        REQUIRE(accounts_1[0].url == "testurl_1");
        REQUIRE(accounts_1[0].notes == "testnotes_1");

        REQUIRE(accounts_2[0].accountName == "testaccount_2");
        REQUIRE(accounts_2[0].login == "testlogin_2");
        REQUIRE_FALSE(accounts_2[0].encryptedPassword.empty());
        REQUIRE(accounts_2[0].url == "testurl_2");
        REQUIRE(accounts_2[0].notes == "testnotes_2");
    }
    
    SECTION("Add multiple accounts for a single user") {
        accountManager.addAccount(userId,
                        "testaccount_1",
                        "testlogin_1",
                        "testpassword_1",
                        "testurl_1",
                        "testnotes_1",
                        dummyKey);

        accountManager.addAccount(userId,
                        "testaccount_2",
                        "testlogin_2",
                        "testpassword_2",
                        "testurl_2",
                        "testnotes_2",
                        dummyKey);

        auto accounts = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts.size() == 2);

        REQUIRE(accounts[0].accountName == "testaccount_1");
        REQUIRE(accounts[0].login == "testlogin_1");
        REQUIRE_FALSE(accounts[0].encryptedPassword.empty());
        REQUIRE(accounts[0].url == "testurl_1");
        REQUIRE(accounts[0].notes == "testnotes_1");

        REQUIRE(accounts[1].accountName == "testaccount_2");
        REQUIRE(accounts[1].login == "testlogin_2");
        REQUIRE_FALSE(accounts[1].encryptedPassword.empty());
        REQUIRE(accounts[1].url == "testurl_2");
        REQUIRE(accounts[1].notes == "testnotes_2");
    }

    SECTION("Update non-existent account") {
        accountManager.addAccount(userId,
                        "testaccount",
                        "testlogin",
                        "testpassword",
                        "testurl",
                        "testnotes",
                        dummyKey);

        auto accounts = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.deleteAccount(userId, accountId);
        auto accounts_1 = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts_1.size() == 0);

        REQUIRE_THROWS_AS(accountManager.updateAccount(userId,
                        accountId,
                        "testaccount",
                        "testlogin",
                        "testpassword",
                        "testurl",
                        "testnotes",
                        dummyKey), std::runtime_error);
    }

    SECTION("Delete non-existent account") {
        accountManager.addAccount(userId,
                        "testaccount",
                        "testlogin",
                        "testpasswordHash",
                        "testurl",
                        "testnotes",
                        dummyKey);

        auto accounts = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts.size() == 1);

        int accountId = accounts[0].id;

        accountManager.deleteAccount(userId, accountId);
        auto accounts_1 = accountManager.getAccountsForUser(userId, dummyKey);
        REQUIRE(accounts_1.size() == 0);
        
        REQUIRE_THROWS_AS(accountManager.deleteAccount(userId, accountId), std::runtime_error);
    }

    SECTION("Get accounts for non-existent user") {
        auto accounts = accountManager.getAccountsForUser(99999, dummyKey);
        REQUIRE(accounts.empty());
    }
}

/*TEST_CASE("EncryptionManager encrypt/decrypt", "[encryption]") {
    std::string testDbPath = ":memory:";
    DatabaseManager dbManager(testDbPath);
    dbManager.initialize();

    UserManager userManager(dbManager.getDb());
    int userId = userManager.registerUser("testuser", "testpass");

    AccountManager accountManager(dbManager, userManager);
}*/