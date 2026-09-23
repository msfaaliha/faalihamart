#pragma once

#include <string>
#include <vector>
#include <memory>

namespace faaliha::faalihamart::repository {

/**
 * @brief Repository interface for database migrations tracking.
 */
class IMigrationRepository {
public:
    virtual ~IMigrationRepository() = default;

    /**
     * @brief Ensure the schema_migrations tracking table exists.
     */
    virtual void EnsureMigrationTable() = 0;

    /**
     * @brief Get all applied migration version numbers in ascending order.
     * @return List of applied version numbers.
     */
    virtual std::vector<int> GetAppliedVersions() = 0;

    /**
     * @brief Execute a migration script and record its version inside a transaction.
     * @param version The numeric version of the migration.
     * @param name The migration filename or identifier.
     * @param sql The SQL script contents.
     */
    virtual void ApplyMigration(int version, const std::string& name, const std::string& sql) = 0;

    /**
     * @brief Execute an arbitrary SQL script (e.g., seed.sql).
     * @param sql Script content.
     */
    virtual void ExecuteRawSql(const std::string& sql) = 0;

    /**
     * @brief Verify database connectivity via trivial query (SELECT 1).
     * @return true if database is responding.
     */
    virtual bool HealthCheck() = 0;
};

} // namespace faaliha::faalihamart::repository
