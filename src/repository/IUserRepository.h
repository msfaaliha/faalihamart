#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "../model/User.h"

namespace faaliha::faalihamart::repository {

/**
 * @brief Repository interface for User entity data access abstraction.
 * All implementations must use strictly parameterized queries.
 */
class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    /**
     * @brief Find a user by their unique primary key.
     * @param id The user identifier.
     * @return Optional containing the User if found.
     */
    virtual std::optional<model::User> FindById(int64_t id) = 0;

    /**
     * @brief Find a user by their unique email address.
     * @param email The user's email address.
     * @return Optional containing the User if found.
     */
    virtual std::optional<model::User> FindByEmail(const std::string& email) = 0;

    /**
     * @brief Create a new user record.
     * @param user The user entity to insert (id_ will be populated).
     * @return The persisted user entity with generated ID.
     */
    virtual model::User Create(const model::User& user) = 0;

    /**
     * @brief Retrieve all users (for Admin oversight).
     * @return Vector of all user records.
     */
    virtual std::vector<model::User> FindAll() = 0;

    /**
     * @brief Count total users by role or total.
     * @param role Optional role filter.
     * @return Total count.
     */
    virtual int64_t Count(std::optional<model::UserRole> role = std::nullopt) = 0;
};

} // namespace faaliha::faalihamart::repository
