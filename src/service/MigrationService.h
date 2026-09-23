#pragma once

#include <memory>
#include <string>
#include "../repository/IMigrationRepository.h"

namespace faaliha::faalihamart::service {

/**
 * @brief Hand-written migration runner executing db/migrations/*.sql sequentially.
 */
class MigrationService {
public:
    explicit MigrationService(std::shared_ptr<repository::IMigrationRepository> migration_repo);

    /**
     * @brief Run unapplied migrations found in migrations_dir.
     * @param migrations_dir Path to migrations folder (e.g. "db/migrations").
     * @return Number of applied migrations.
     */
    int RunMigrations(const std::string& migrations_dir = "db/migrations");

    /**
     * @brief Apply seed data from seed_file if database is fresh.
     * @param seed_file Path to seed SQL script (e.g. "db/seed.sql").
     */
    void ApplySeedDataIfFresh(const std::string& seed_file = "db/seed.sql");

    /**
     * @brief Test database connectivity (SELECT 1).
     */
    bool HealthCheck();

private:
    std::shared_ptr<repository::IMigrationRepository> migration_repo_;
};

} // namespace faaliha::faalihamart::service
