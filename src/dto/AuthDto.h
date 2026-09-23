#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../model/User.h"

namespace faaliha::faalihamart::dto {

/**
 * @brief Login request DTO.
 */
struct LoginRequestDto {
    std::string email;
    std::string password;
};

/**
 * @brief Register request DTO.
 */
struct RegisterRequestDto {
    std::string name;
    std::string email;
    std::string password;
    std::string role{"BUYER"}; // BUYER or SELLER
};

/**
 * @brief User response DTO - Never leaks password_hash.
 */
class UserResponseDto {
public:
    int64_t id{0};
    std::string name;
    std::string email;
    std::string role;
    std::string created_at;

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"id", id},
            {"name", name},
            {"email", email},
            {"role", role},
            {"created_at", created_at}
        };
    }

    // Fluent Builder Pattern
    class Builder {
    public:
        Builder& WithId(int64_t id) { dto_.id = id; return *this; }
        Builder& WithName(std::string name) { dto_.name = std::move(name); return *this; }
        Builder& WithEmail(std::string email) { dto_.email = std::move(email); return *this; }
        Builder& WithRole(std::string role) { dto_.role = std::move(role); return *this; }
        Builder& WithCreatedAt(std::string created_at) { dto_.created_at = std::move(created_at); return *this; }
        UserResponseDto Build() { return std::move(dto_); }
    private:
        UserResponseDto dto_;
    };

    static UserResponseDto FromEntity(const model::User& user) {
        return Builder()
            .WithId(user.id_)
            .WithName(user.name_)
            .WithEmail(user.email_)
            .WithRole(model::RoleToString(user.role_))
            .WithCreatedAt(user.created_at_)
            .Build();
    }
};

} // namespace faaliha::faalihamart::dto
