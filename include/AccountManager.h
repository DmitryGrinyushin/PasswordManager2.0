#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

struct Account {
    int id;
    std::string accountName;
    std::string login;
    std::string passwordHash;
    std::string notes;
    std::string createdAt;
    std::string updatedAt;
};

class AccountManager {
    sqlite3* db;

public:
    AccountManager(sqlite3* database);

    void addAccount(int userId,
                    const std::string& accountName,
                    const std::string& login,
                    const std::string& passwordHash,
                    const std::string& notes);

    std::vector<Account> getAccountsForUser(int userId);

    void updateAccount(int accountId,
                        const std::string& accountName,
                        const std::string& login,
                        const std::string& passwordHash,
                        const std::string& notes);

    void deleteAccount(int accountId);
};