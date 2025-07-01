#include "Logger.h"
#include "DatabaseManager.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <sqlite3.h>

int main() {
    try {
        Logger& logger = Logger::getInstance();
        logger.log(LogLevel::INFO, "Starting application");

        DatabaseManager dbManager("data/passdb.sqlite");
        dbManager.initialize();

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}