#include "SQLiteUserRepository.h"
#include "SQLiteHelper.h"
#include <stdexcept>

namespace faaliha::faalihamart::repository::sqlite {

SQLiteUserRepository::SQLiteUserRepository(sqlite3* db) : db_(db) {}

std::optional<model::User> SQLiteUserRepository::FindById(int64_t id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "SELECT id, name, email, password_hash, role, created_at FROM users WHERE id = ?;";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::User user;
        user.id_ = sqlite3_column_int64(stmt.get(), 0);
        user.name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
        user.email_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        user.password_hash_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 3));
        user.role_ = model::StringToRole(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4)));
        user.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        return user;
    }
    return std::nullopt;
}

std::optional<model::User> SQLiteUserRepository::FindByEmail(const std::string& email) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "SELECT id, name, email, password_hash, role, created_at FROM users WHERE email = ?;";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_text(stmt.get(), 1, email.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::User user;
        user.id_ = sqlite3_column_int64(stmt.get(), 0);
        user.name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
        user.email_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        user.password_hash_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 3));
        user.role_ = model::StringToRole(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4)));
        user.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        return user;
    }
    return std::nullopt;
}

model::User SQLiteUserRepository::Create(const model::User& user) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "INSERT INTO users (name, email, password_hash, role) VALUES (?, ?, ?, ?);";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_text(stmt.get(), 1, user.name_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 2, user.email_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 3, user.password_hash_.c_str(), -1, SQLITE_TRANSIENT);
    std::string role_str = model::RoleToString(user.role_);
    sqlite3_bind_text(stmt.get(), 4, role_str.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        throw std::runtime_error("Failed to insert user: " + std::string(sqlite3_errmsg(db_)));
    }

    int64_t new_id = sqlite3_last_insert_rowid(db_);
    model::User created = user;
    created.id_ = new_id;
    return created;
}

std::vector<model::User> SQLiteUserRepository::FindAll() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "SELECT id, name, email, password_hash, role, created_at FROM users ORDER BY id ASC;";
    auto stmt = PrepareStatement(db_, sql);

    std::vector<model::User> users;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::User u;
        u.id_ = sqlite3_column_int64(stmt.get(), 0);
        u.name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
        u.email_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        u.password_hash_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 3));
        u.role_ = model::StringToRole(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4)));
        u.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        users.push_back(u);
    }
    return users;
}

int64_t SQLiteUserRepository::Count(std::optional<model::UserRole> role) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    std::string sql = "SELECT COUNT(*) FROM users";
    if (role.has_value()) {
        sql += " WHERE role = ?;";
    } else {
        sql += ";";
    }
    auto stmt = PrepareStatement(db_, sql);
    if (role.has_value()) {
        std::string role_str = model::RoleToString(role.value());
        sqlite3_bind_text(stmt.get(), 1, role_str.c_str(), -1, SQLITE_TRANSIENT);
    }

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return sqlite3_column_int64(stmt.get(), 0);
    }
    return 0;
}

} // namespace faaliha::faalihamart::repository::sqlite
