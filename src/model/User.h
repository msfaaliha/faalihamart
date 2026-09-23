#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

namespace faaliha::faalihamart::model {

enum class UserRole {
    BUYER,
    SELLER,
    ADMIN
};

inline std::string RoleToString(UserRole role) {
    switch (role) {
        case UserRole::BUYER: return "BUYER";
        case UserRole::SELLER: return "SELLER";
        case UserRole::ADMIN: return "ADMIN";
    }
    return "BUYER";
}

inline UserRole StringToRole(const std::string& str) {
    if (str == "ADMIN") return UserRole::ADMIN;
    if (str == "SELLER") return UserRole::SELLER;
    return UserRole::BUYER;
}

/**
 * @brief User domain model.
 */
struct User {
    int64_t id_{0};
    std::string name_;
    std::string email_;
    std::string password_hash_;
    UserRole role_{UserRole::BUYER};
    std::string created_at_;

    [[nodiscard]] bool IsAdmin() const noexcept { return role_ == UserRole::ADMIN; }
    [[nodiscard]] bool IsSeller() const noexcept { return role_ == UserRole::SELLER; }
    [[nodiscard]] bool IsBuyer() const noexcept { return role_ == UserRole::BUYER; }
};

} // namespace faaliha::faalihamart::model
