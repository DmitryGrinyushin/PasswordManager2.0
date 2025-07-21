#include "Logger.h"
#include "DatabaseManager.h"
#include "UserManager.h"
#include "AccountManager.h"
#include "PasswordHasher.h"
#include "PasswordGenerator.h"
#include "EnvUtils.h"
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
        int userId = userManager.loginUser(username, password);
        if (userId == -1) throw std::runtime_error("Login failed");

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
                userId,
                accounts[0].id,
                "GitHub (updated)",
                "updated_login@github.com",
                accPassHash,
                "Updated notes"
            );
        }

        accounts = accountManager.getAccountsForUser(userId);
         std::cout << "Updated accounts for user ID " << userId << ":\n";
         for (const auto& acc : accounts) {
            std::cout << "ID: " << acc.id
                      << ", Name: " << acc.accountName
                      << ", Login: " << acc.login
                      << ", Notes: " << acc.notes << "\n";
        }

        // delete account
        if (!accounts.empty()) {
            accountManager.deleteAccount(userId, accounts[0].id);
        }

        // delete test user
        userManager.deleteUser(username);

        logger.log(LogLevel::INFO, "Test completed successfully");
        
        // PasswordGenerator test
        PasswordGenerator gen1(PasswordType::digits, 8, 12);
        PasswordGenerator gen2(PasswordType::letters, 8, 12);
        PasswordGenerator gen3(PasswordType::capitalLetters, 8, 12);
        PasswordGenerator gen4(PasswordType::symbols, 8, 12);
        PasswordGenerator gen5(PasswordType::digitsAndLetters, 8, 12);
        PasswordGenerator gen6(PasswordType::allChars, 8, 12);

        std::cout << "Digits:           " << gen1() << "\n";
        std::cout << "Letters:          " << gen2() << "\n";
        std::cout << "Capital Letters:  " << gen3() << "\n";
        std::cout << "Symbols:          " << gen4() << "\n";
        std::cout << "Digits+Letters:   " << gen5() << "\n";
        std::cout << "All Characters:   " << gen6() << "\n";
        // =========== to be removed ============

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}