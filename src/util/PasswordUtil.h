#pragma once

#include <string>

namespace faaliha::faalihamart::util {

/**
 * @brief Utilities for secure password hashing and verification via Argon2id.
 */
class PasswordUtil {
public:
    /**
     * @brief Hashes a plain password using Argon2id.
     * @param password Plain-text password.
     * @return Formatted hash string.
     */
    static std::string HashPassword(const std::string& password);

    /**
     * @brief Verifies whether the password matches the hash.
     * @param hash Stored password hash.
     * @param password Plain-text password to verify.
     * @return true if valid, false otherwise.
     */
    static bool VerifyPassword(const std::string& hash, const std::string& password);
};

} // namespace faaliha::faalihamart::util
