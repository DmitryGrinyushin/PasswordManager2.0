#include "CLI.h"
#include "PasswordHasher.h"
#include <iostream>
#include <limits>

void clearInput() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int showMainMenu() {
    std::cout << "\n=== Password Manager ===\n";
    std::cout << "1. Register\n";
    std::cout << "2. Login\n";
    std::cout << "0. Exit\n";
    std::cout << "Select option: ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    clearInput();
    return choice;
}

int showUserMenu() {
    std::cout << "\n--- Account Management ---\n";
    std::cout << "1. Add account\n";
    std::cout << "2. View accounts\n";
    std::cout << "3. Update account\n";
    std::cout << "4. Delete account\n";
    std::cout << "0. Logout\n";
    std::cout << "Select option: ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    clearInput();
    return choice;
}

void runCLI(DatabaseManager& dbManager, UserManager& userManager, AccountManager& accountManager) {
    while (true) {
        int choice = showMainMenu();

        if (choice == -1) {
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 0) break;

        std::string username, password;
        std::cout << "Username: ";
        std::getline(std::cin, username);
        std::cout << "Password: ";
        std::getline(std::cin, password);

        int userId = -1;

        try {
            if (choice == 1) {
                userId = userManager.registerUser(username, password);
                std::cout << "User registered. Please login.\n";
                continue;
            } else if (choice == 2) {
                userId = userManager.loginUser(username, password);
                std::cout << "Login successful.\n";
            } else {
                std::cout << "Invalid option.\n";
                continue;
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
            continue;
        }

        while (true) {
            int subChoice = showUserMenu();

            if (subChoice == -1) {
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            if (subChoice == 0) break;

            try {
                if (subChoice == 1) {
                    std::string name, login, pass, notes;
                    std::cout << "Account name: "; std::getline(std::cin, name);
                    std::cout << "Login: "; std::getline(std::cin, login);
                    std::cout << "Password: "; std::getline(std::cin, pass);
                    std::cout << "Notes: "; std::getline(std::cin, notes);
                    std::string hashed = PasswordHasher::hashPassword(pass, "somesalt");
                    accountManager.addAccount(userId, name, login, hashed, notes);
                } else if (subChoice == 2) {
                    auto accounts = accountManager.getAccountsForUser(userId);
                    for (const auto& acc : accounts) {
                        std::cout << "[" << acc.id << "] " << acc.accountName << " | "
                                  << acc.login << " | " << acc.notes << "\n";
                    }
                } else if (subChoice == 3) {
                    int id;
                    std::string name, login, pass, notes;
                    std::cout << "Account ID to update: "; std::cin >> id; clearInput();
                    std::cout << "New name: "; std::getline(std::cin, name);
                    std::cout << "New login: "; std::getline(std::cin, login);
                    std::cout << "New password: "; std::getline(std::cin, pass);
                    std::cout << "New notes: "; std::getline(std::cin, notes);
                    std::string hashed = PasswordHasher::hashPassword(pass, "somesalt");
                    accountManager.updateAccount(userId, id, name, login, hashed, notes);
                } else if (subChoice == 4) {
                    int id;
                    std::cout << "Account ID to delete: "; std::cin >> id; clearInput();
                    accountManager.deleteAccount(userId, id);
                } else {
                    std::cout << "Invalid option.\n";
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
    }

    std::cout << "Exiting Password Manager.\n";
}