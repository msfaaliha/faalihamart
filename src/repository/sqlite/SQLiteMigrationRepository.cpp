#include "SQLiteMigrationRepository.h"
#include "SQLiteHelper.h"
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::repository::sqlite {

SQLiteMigrationRepository::SQLiteMigrationRepository(sqlite3* db) : db_(db) {}

void SQLiteMigrationRepository::EnsureMigrationTable() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "CREATE TABLE IF NOT EXISTS schema_migrations ("
        "    version INTEGER PRIMARY KEY,"
        "    name TEXT NOT NULL,"
        "    applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");";

    char* errmsg = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::string err = errmsg ? errmsg : "Unknown error";
        sqlite3_free(errmsg);
        throw std::runtime_error("Failed to ensure schema_migrations table: " + err);
    }
}

std::vector<int> SQLiteMigrationRepository::GetAppliedVersions() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "SELECT version FROM schema_migrations ORDER BY version ASC;";
    auto stmt = PrepareStatement(db_, sql);

    std::vector<int> versions;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        versions.push_back(sqlite3_column_int(stmt.get(), 0));
    }
    return versions;
}

void SQLiteMigrationRepository::ApplyMigration(int version, const std::string& name, const std::string& sql) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    char* errmsg = nullptr;
    if (sqlite3_exec(db_, "BEGIN TRANSACTION;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::string err = errmsg ? errmsg : "Transaction error";
        sqlite3_free(errmsg);
        throw std::runtime_error(err);
    }

    try {
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
            std::string err = errmsg ? errmsg : "Migration execution error";
            sqlite3_free(errmsg);
            throw std::runtime_error("Migration script failed: " + err);
        }

        const std::string record_sql = "INSERT INTO schema_migrations (version, name) VALUES (?, ?);";
        auto stmt = PrepareStatement(db_, record_sql);
        sqlite3_bind_int(stmt.get(), 1, version);
        sqlite3_bind_text(stmt.get(), 2, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
            throw std::runtime_error("Failed to record migration version: " + std::string(sqlite3_errmsg(db_)));
        }

        if (sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
            std::string err = errmsg ? errmsg : "Commit error";
            sqlite3_free(errmsg);
            throw std::runtime_error(err);
        }
    } catch (...) {
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        throw;
    }
}

void SQLiteMigrationRepository::ExecuteRawSql(const std::string& sql) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    char* errmsg = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::string err = errmsg ? errmsg : "Error executing raw SQL";
        sqlite3_free(errmsg);
        throw std::runtime_error(err);
    }
}

bool SQLiteMigrationRepository::HealthCheck() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    auto stmt = PrepareStatement(db_, "SELECT 1;");
    return sqlite3_step(stmt.get()) == SQLITE_ROW;
}

} // namespace faaliha::faalihamart::repository::sqlite
