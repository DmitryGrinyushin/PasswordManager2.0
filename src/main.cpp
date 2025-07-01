#include "Logger.h"
#include "DatabaseManager.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <sqlite3.h>

namespace fs = std::filesystem;

bool fileExists(const std::string& filename) {
    return (fs::exists(filename));
}

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