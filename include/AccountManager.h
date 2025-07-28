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
    std::string passwordHash;
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

    void addAccount(int userId,
                    const std::string& accountName,
                    const std::string& login,
                    const std::string& passwordHash,
                    const std::string& url,
                    const std::string& notes);

    std::vector<Account> getAccountsForUser(int userId);

    void updateAccount(int userId,
                        int accountId,
                        const std::string& accountName,
                        const std::string& login,
                        const std::string& passwordHash,
                        const std::string& url,
                        const std::string& notes);

    void deleteAccount(int userId, int accountId);
};