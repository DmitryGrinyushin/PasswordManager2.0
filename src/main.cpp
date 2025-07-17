#include "Logger.h"
#include "DatabaseManager.h"
#include "UserManager.h"
#include "AccountManager.h"
#include "StatementWrapper.h"
#include "EnvUtils.h"
#include "PasswordHasher.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <sqlite3.h>

int main() {

    try {
        Logger& logger = Logger::getInstance();
        logger.log(LogLevel::INFO, "Starting application");

        std::string dbPath = getEnvOrDefault("PASSWORD_MANAGER_DB_PATH", "data/passdb.sqlite");
        DatabaseManager dbManager(dbPath);
        dbManager.initialize();

        // TODO: Temporary testing functionality - remove before production
        // UserManager test
        UserManager userManager(dbManager.getDb());
        const std::string username = "testuser";
        const std::string password = "password123";

        if (!userManager.userExists(username)) {
            userManager.registerUser(username, password);
        }
        userManager.loginUser(username, password);

        // get ID
        const char* sql = "SELECT id FROM users WHERE username = ?;";
        StatementWrapper stmt(dbManager.getDb(), sql);
        sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);
        int rc = stmt.step();
        if (rc != SQLITE_ROW) {
            throw std::runtime_error("Failed to get user ID.");
        }
        int userId = sqlite3_column_int(stmt.get(), 0);

        // AccountManager test
        AccountManager accountManager(dbManager.getDb());

        std::string accName = "GitHub";
        std::string accLogin = "testuser@github.com";
        std::string accPassHash = PasswordHasher::hashPassword("github_password", "somesalt");
        std::string accNotes = "My GitHub account";

        accountManager.addAccount(userId, accName, accLogin, accPassHash, accNotes);

        std::vector<Account> accounts = accountManager.getAccountsForUser(userId);
        std::cout << "Accounts for user ID " << userId << ":\n";
        for (const auto& acc : accounts) {
            std::cout << "ID: " << acc.id
                      << ", Name: " << acc.accountName
                      << ", Login: " << acc.login
                      << ", Notes: " << acc.notes << "\n";
        }

        // update account
        if (!accounts.empty()) {
            accountManager.updateAccount(
                accounts[0].id,
                "GitHub (updated)",
                "updated_login@github.com",
                accPassHash,
                "Updated notes"
            );
        }

        // delete account
        if (!accounts.empty()) {
            accountManager.deleteAccount(accounts[0].id);
        }

        // delete test user
        userManager.deleteUser(username);

        logger.log(LogLevel::INFO, "Test completed successfully");
        // ======

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}