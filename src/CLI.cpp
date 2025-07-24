#include "CLI.h"
#include "PasswordHasher.h"
#include "PasswordGenerator.h"
#include <iostream>
#include <limits>

void clearInput() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readIntSafe(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number.\n";
    }
}

int showMainMenu() {
    std::cout << "\n=== Password Manager ===\n";
    std::cout << "1. Register\n";
    std::cout << "2. Login\n";
    std::cout << "0. Exit\n";
    int choice = readIntSafe("Select option: ");
    return choice;
}

int showUserMenu() {
    std::cout << "\n--- Account Management ---\n";
    std::cout << "1. Add account\n";
    std::cout << "2. View accounts\n";
    std::cout << "3. Update account\n";
    std::cout << "4. Delete account\n";
    std::cout << "0. Logout\n";
    int choice = readIntSafe("Select option: ");
    return choice;
}

// generate password
std::string createPass() {
    std::cout << "1. Enter password manually\n";
    std::cout << "2. Generate password automatically\n";

    int passChoice = readIntSafe("Select option: ");

    std::string pass;
    if (passChoice == 1) {
        std::cout << "Enter password: "; std::getline(std::cin, pass);
    }
    else if (passChoice == 2) {
        std::cout << "\n--- Password type ---\n";
        std::cout << "1. Numbers only\n";
        std::cout << "2. All characters (default option)\n";
        std::cout << "3. Letters only\n";
        std::cout << "4. Capital letters only\n";

        int typeChoice = readIntSafe("Select option: ");

        PasswordType type;
        switch (typeChoice) {
            case 1: type = PasswordType::digits; break;
            case 2: type = PasswordType::allChars; break;
            case 3: type = PasswordType::letters; break;
            case 4: type = PasswordType::uppercaseLetters; break;
            default:
                std::cout << "Invalid option. Using default.\n";
                type = PasswordType::allChars;
                break;
        }

        int minLen = readIntSafe("Min password length: ");
        int maxLen = readIntSafe("Max password length: ");

        pass = PasswordGenerator (type, minLen, maxLen)();
        std::cout << "Generated password: " << pass << "\n";
    } else {
        std::cout << "Invalid choice. Returning empty password.\n";
    }
    return pass;
}

void runCLI(DatabaseManager& dbManager, UserManager& userManager, AccountManager& accountManager) {
    while (true) {
        int choice = showMainMenu();

        if (choice == 0) break;

        std::string username, password;
        std::cout << "Username: ";
        std::getline(std::cin, username);

        int userId = -1;

        try {
            if (choice == 1) {
                // insert createPass here
                password = createPass();
                userId = userManager.registerUser(username, password);
                std::cout << "User registered. Please login.\n";
                continue;
            } else if (choice == 2) {
                std::cout << "Password: ";
                std::getline(std::cin, password);

                userId = userManager.loginUser(username, password);
                if (userId == -1) {
                    std::cout << "Login failed.\n";
                    continue;
                }
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

            if (subChoice == 0) break;

            try {
                if (subChoice == 1) {
                    std::string name, login, pass, notes;
                    std::cout << "Account name: "; std::getline(std::cin, name);
                    std::cout << "Login: "; std::getline(std::cin, login);
                    // insert createPass here
                    pass = createPass();
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
                    // insert createPass here
                    pass = createPass();
                    std::cout << "New notes: "; std::getline(std::cin, notes);
                    std::string hashed = PasswordHasher::hashPassword(pass, "somesalt");
                    accountManager.updateAccount(userId, id, name, login, hashed, notes);
                } else if (subChoice == 4) {
                    int id = readIntSafe("Account ID to delete: ");
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