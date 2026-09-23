#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <sqlite3.h>

namespace faaliha::faalihamart::repository::sqlite {

struct SQLiteStmtDeleter {
    void operator()(sqlite3_stmt* stmt) const {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
    }
};

using ScopedStmt = std::unique_ptr<sqlite3_stmt, SQLiteStmtDeleter>;

struct SQLiteDbDeleter {
    void operator()(sqlite3* db) const {
        if (db) {
            sqlite3_close_v2(db);
        }
    }
};

using ScopedDb = std::unique_ptr<sqlite3, SQLiteDbDeleter>;

/**
 * @brief Helper to prepare parameterized SQLite statements with RAII.
 */
inline ScopedStmt PrepareStatement(sqlite3* db, const std::string& sql) {
    sqlite3_stmt* raw_stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &raw_stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        throw std::runtime_error("Failed to prepare statement: " + sql + " | Error: " + err);
    }
    return ScopedStmt(raw_stmt);
}

} // namespace faaliha::faalihamart::repository::sqlite
