#include "Logger.h"
#include "DatabaseManager.h"
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

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}