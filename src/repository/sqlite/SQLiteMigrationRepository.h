#pragma once

#include "../IMigrationRepository.h"
#include <sqlite3.h>
#include <mutex>

namespace faaliha::faalihamart::repository::sqlite {

/**
 * @brief SQLite implementation of IMigrationRepository.
 */
class SQLiteMigrationRepository : public IMigrationRepository {
public:
    explicit SQLiteMigrationRepository(sqlite3* db);

    void EnsureMigrationTable() override;
    std::vector<int> GetAppliedVersions() override;
    void ApplyMigration(int version, const std::string& name, const std::string& sql) override;
    void ExecuteRawSql(const std::string& sql) override;
    bool HealthCheck() override;

private:
    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
};

} // namespace faaliha::faalihamart::repository::sqlite
