#include "AccountManager.h"
#include "Logger.h"
#include "StatementWrapper.h"
#include "EncryptionManager.h"
#include <stdexcept>
#include <iostream>

AccountManager::AccountManager(DatabaseManager& dbManager, UserManager& userManager)
        : dbManager(dbManager), userManager(userManager) {}

void AccountManager::addAccount(int userId,
                        const std::string& accountName,
                        const std::string& login,
                        const std::string& plainPassword,
                        const std::string& url,
                        const std::string& notes,
                        const std::vector<unsigned char>& key) {

    std::string encryptedPassword = EncryptionManager::encryptField(plainPassword, key);
    std::cout << "[ENCRYPTED] " << encryptedPassword << "\n"; // test
    const char* sql = 
        "INSERT INTO accounts (user_id, account_name, login, password_encrypted, url, notes) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    StatementWrapper stmt(dbManager.getDb(), sql);

    sqlite3_bind_int(stmt.get(), 1, userId);
    sqlite3_bind_text(stmt.get(), 2, accountName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 3, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 4, encryptedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 5, url.c_str(), -1, SQLITE_STATIC);   
    sqlite3_bind_text(stmt.get(), 6, notes.c_str(), -1, SQLITE_STATIC);

    int rc = stmt.step();
    if (rc != SQLITE_DONE) {
        std::string errorMessage = "Error inserting account: " + std::string(sqlite3_errmsg(dbManager.getDb()));
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    int accountId = static_cast<int>(sqlite3_last_insert_rowid(dbManager.getDb()));

    std::cout << "Account added successfully.\n"; // to be removed before production
    Logger::getInstance().log(LogLevel::INFO, "Account \"" + accountName + "\" with ID " + std::to_string(accountId) + " added for user ID " + std::to_string(userId));
}

std::vector<Account> AccountManager::getAccountsForUser(int userId, const std::vector<unsigned char>& key) {
    std::vector<Account> accounts;

    const char* sql =
        "SELECT id, account_name, login, password_encrypted, url, notes, created_at, updated_at "
        "FROM accounts WHERE user_id = ? ORDER BY id;";

    StatementWrapper stmt(dbManager.getDb(), sql);

    sqlite3_bind_int(stmt.get(), 1, userId);

    while (true) {
        int rc = stmt.step();
        if (rc == SQLITE_ROW) {
            Account account;

            account.id = sqlite3_column_int(stmt.get(), 0);
            account.accountName = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
            account.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
            account.encryptedPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 3));
            account.url = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
            account.notes = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
            account.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
            account.updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 7));
            
            accounts.push_back(account);
        }
        else if (rc == SQLITE_DONE) {
            break;
        }
        else {
            std::string errorMessage = "Error selecting accounts: " + std::string(sqlite3_errmsg(dbManager.getDb()));
            Logger::getInstance().log(LogLevel::ERROR, errorMessage);
            throw std::runtime_error(errorMessage);
        }
    }

    Logger::getInstance().log(LogLevel::INFO, "Fetched " + std::to_string(accounts.size()) + " accounts for user ID " + std::to_string(userId));
    return accounts;
}

void AccountManager::updateAccount(int userId,
                        int accountId,
                        const std::string& accountName,
                        const std::string& login,
                        const std::string& plainPassword,
                        const std::string& url,
                        const std::string& notes,
                        const std::vector<unsigned char>& key) {

    std::string encryptedPassword = EncryptionManager::encryptField(plainPassword, key);

    const char* sql =
        "UPDATE accounts SET "
        "account_name = ?, "
        "login = ?, "
        "password_encrypted = ?, "
        "url = ?, "
        "notes = ?, "
        "updated_at = CURRENT_TIMESTAMP "
        "WHERE id = ? AND user_id = ?;";

    StatementWrapper stmt(dbManager.getDb(), sql);

    sqlite3_bind_text(stmt.get(), 1, accountName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 2, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 3, encryptedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 4, url.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 5, notes.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt.get(), 6, accountId); // Identification (WHERE id)
    sqlite3_bind_int(stmt.get(), 7, userId);

    int rc = stmt.step();
    if (rc != SQLITE_DONE) {
        std::string errorMessage = "Error updating account: " + std::string(sqlite3_errmsg(dbManager.getDb()));
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    int changes = sqlite3_changes(dbManager.getDb());
    if (changes == 0) {
        std::string warning = "Update failed: account with ID " + std::to_string(accountId) + " not found.";
        Logger::getInstance().log(LogLevel::WARNING, warning);
        throw std::runtime_error(warning);
    }

    std::cout << "Account updated successfully.\n";
    Logger::getInstance().log(LogLevel::INFO, "User ID " + std::to_string(userId) + " updated account ID " + std::to_string(accountId));
}

void AccountManager::deleteAccount(int userId, int accountId)
{
    const char* sql = "DELETE FROM accounts WHERE id = ? AND user_id = ?;";

    StatementWrapper stmt(dbManager.getDb(), sql);

    sqlite3_bind_int(stmt.get(), 1, accountId); // ID connection
    sqlite3_bind_int(stmt.get(), 2, userId);

    int rc = stmt.step();
    if (rc != SQLITE_DONE) {
        std::string errorMessage = "Error deleting account: " + std::string(sqlite3_errmsg(dbManager.getDb()));
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }

    int changes = sqlite3_changes(dbManager.getDb());
    if (changes == 0) {
        std::string warning = "Delete failed: account with ID " + std::to_string(accountId) + " not found.";
        Logger::getInstance().log(LogLevel::WARNING, warning);
        throw std::runtime_error(warning);
    }

    std::cout << "Account deleted successfully.\n";
    Logger::getInstance().log(LogLevel::INFO, "User ID " + std::to_string(userId) + " deleted account ID " + std::to_string(accountId));
}