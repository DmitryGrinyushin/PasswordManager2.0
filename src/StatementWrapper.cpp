#include "StatementWrapper.h"
#include "Logger.h"
#include <string>
#include <sqlite3.h>
#include <stdexcept>

StatementWrapper::StatementWrapper (sqlite3* db, const char* sql) {
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string errorMessage = std::string("Database request generation error: ") + sqlite3_errmsg(db);
        Logger::getInstance().log(LogLevel::ERROR, errorMessage);
        throw std::runtime_error(errorMessage);
    }
}
StatementWrapper::~StatementWrapper () {
    if (stmt) sqlite3_finalize(stmt);
}

sqlite3_stmt* StatementWrapper::get() {
    return stmt;
}

int StatementWrapper::step() {
    return sqlite3_step(stmt);
}