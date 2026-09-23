#pragma once

#include "../IUserRepository.h"
#include <sqlite3.h>
#include <mutex>

namespace faaliha::faalihamart::repository::sqlite {

/**
 * @brief SQLite implementation of IUserRepository using parameterized prepared statements.
 */
class SQLiteUserRepository : public IUserRepository {
public:
    explicit SQLiteUserRepository(sqlite3* db);

    std::optional<model::User> FindById(int64_t id) override;
    std::optional<model::User> FindByEmail(const std::string& email) override;
    model::User Create(const model::User& user) override;
    std::vector<model::User> FindAll() override;
    int64_t Count(std::optional<model::UserRole> role = std::nullopt) override;

private:
    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
};

} // namespace faaliha::faalihamart::repository::sqlite
