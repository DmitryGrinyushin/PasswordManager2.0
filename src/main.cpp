#include "Logger.h"
#include "DatabaseManager.h"
#include "UserManager.h"
#include "AccountManager.h"
#include "PasswordHasher.h"
#include "PasswordGenerator.h"
#include "EnvUtils.h"
#include "CLI.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <sqlite3.h>
#include "EncryptionManager.h"

int main() {

    try {
        Logger& logger = Logger::getInstance();
        logger.log(LogLevel::INFO, "Starting application");

    // Encryption test - remove before production
    {
        std::cout << "\n🔐 Running encryption test...\n";
        std::string original = "TestPassword123!";
        std::vector<unsigned char> key = EncryptionManager::generateRandomBytes(32);

    try {
        std::string encrypted = EncryptionManager::encryptField(original, key);
        std::string decrypted = EncryptionManager::decryptField(encrypted, key);

        std::cout << "Original:   " << original << "\n";
        std::cout << "Encrypted:  " << encrypted << "\n";
        std::cout << "Decrypted:  " << decrypted << "\n";

        if (original == decrypted) {
            std::cout << "Success: Decrypted text matches original\n";
        } else {
            std::cout << "Mismatch: Decrypted text does not match original\n";
        }

        // Addition check: invalid key
        key[0] ^= 0xFF;
        try {
            std::string failed = EncryptionManager::decryptField(encrypted, key);
            std::cout << "Error: Decryption with wrong key unexpectedly succeeded!\n";
        } catch (const std::exception& ex) {
            std::cout << "Correctly failed with wrong key: " << ex.what() << "\n";
        }

    } catch (const std::exception& ex) {
        std::cerr << "Encryption test failed: " << ex.what() << "\n";
    }
    }
    //=== to be removed ===

        std::string dbPath = getEnvOrDefault("PASSWORD_MANAGER_DB_PATH", "data/passdb.sqlite");
        DatabaseManager dbManager(dbPath);
        dbManager.initialize();

        UserManager userManager(dbManager.getDb());
        AccountManager accountManager(dbManager, userManager);

        runCLI(dbManager, userManager, accountManager);

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}