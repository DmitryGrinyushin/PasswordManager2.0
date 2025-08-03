#pragma once

#include "DatabaseManager.h"
#include "UserManager.h"
#include <string>
#include <vector>
#include <sqlite3.h>

struct Account {
    int id;
    std::string accountName;
    std::string login;
    std::string encryptedPassword;
    std::string url;
    std::string notes;
    std::string createdAt;
    std::string updatedAt;
};

class AccountManager {
    DatabaseManager& dbManager;
    UserManager& userManager;

public:
    AccountManager(DatabaseManager& dbManager, UserManager& userManager);

    AccountManager(const AccountManager&) = delete;
    AccountManager& operator=(const AccountManager&) = delete;
    
    AccountManager(AccountManager&&) = delete;
    AccountManager& operator=(AccountManager&&) = delete;

    void addAccount(int userId,
                        const std::string& accountName,
                        const std::string& login,
                        const std::string& plainPassword,
                        const std::string& url,
                        const std::string& notes,
                        const std::vector<unsigned char>& key);

    std::vector<Account> getAccountsForUser(int userId, const std::vector<unsigned char>& key);

    void updateAccount(int userId,
                        int accountId,
                        const std::string& accountName,
                        const std::string& login,
                        const std::string& plainPassword,
                        const std::string& url,
                        const std::string& notes,
                        const std::vector<unsigned char>& key);

    void deleteAccount(int userId, int accountId);
};