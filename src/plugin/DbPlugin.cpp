#include "DbPlugin.h"
#include "../service/MigrationService.h"
#include "../repository/RepositoryFactory.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace faaliha::faalihamart::plugin {

#if HAS_DROGON_PLUGIN
DbPlugin* DbPlugin::instance_ = nullptr;

DbPlugin& DbPlugin::GetInstance() {
    if (!instance_) {
        throw std::runtime_error("DbPlugin has not been initialized by Drogon runtime");
    }
    return *instance_;
}

void DbPlugin::initAndStart(const Json::Value& config) {
    instance_ = this;
    std::string db_path = config.get("database_file", "faalihamart.db").asString();
    spdlog::info("DbPlugin: initializing SQLite database pool at '{}'", db_path);

    int rc = sqlite3_open_v2(
        db_path.c_str(),
        &db_,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr
    );

    if (rc != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "Failed to open SQLite database";
        spdlog::critical("DbPlugin: database open failed: {}", err);
        throw std::runtime_error("Failed to initialize database: " + err);
    }

    // Enable foreign keys and WAL mode for high performance
    sqlite3_exec(db_, "PRAGMA foreign_keys = ON; PRAGMA journal_mode = WAL;", nullptr, nullptr, nullptr);

    // Run migrations on startup
    auto migration_repo = repository::RepositoryFactory::CreateMigrationRepository(db_);
    service::MigrationService migration_svc(migration_repo);
    int applied = migration_svc.RunMigrations("db/migrations");
    spdlog::info("DbPlugin: migrations completed. Total newly applied: {}", applied);

    // Apply seed data if fresh install
    auto user_repo = repository::RepositoryFactory::CreateUserRepository(db_);
    if (user_repo->Count() == 0) {
        spdlog::info("DbPlugin: fresh database detected, applying seed.sql");
        migration_svc.ApplySeedDataIfFresh("db/seed.sql");
    }

    spdlog::info("DbPlugin: database ready for requests");
}

void DbPlugin::shutdown() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    if (db_) {
        spdlog::info("DbPlugin: closing database connections");
        sqlite3_close_v2(db_);
        db_ = nullptr;
    }
}

sqlite3* DbPlugin::GetDb() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    return db_;
}
#else
DbPlugin& DbPlugin::GetInstance() {
    static DbPlugin inst;
    return inst;
}

void DbPlugin::Init(const std::string& db_path) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    if (db_) return;

    int rc = sqlite3_open_v2(
        db_path.c_str(),
        &db_,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr
    );

    if (rc != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "Failed to open database";
        throw std::runtime_error(err);
    }

    sqlite3_exec(db_, "PRAGMA foreign_keys = ON; PRAGMA journal_mode = WAL;", nullptr, nullptr, nullptr);

    auto migration_repo = repository::RepositoryFactory::CreateMigrationRepository(db_);
    service::MigrationService migration_svc(migration_repo);
    migration_svc.RunMigrations("db/migrations");

    auto user_repo = repository::RepositoryFactory::CreateUserRepository(db_);
    if (user_repo->Count() == 0) {
        migration_svc.ApplySeedDataIfFresh("db/seed.sql");
    }
}

void DbPlugin::Shutdown() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    if (db_) {
        sqlite3_close_v2(db_);
        db_ = nullptr;
    }
}

sqlite3* DbPlugin::GetDb() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    return db_;
}
#endif

} // namespace faaliha::faalihamart::plugin
