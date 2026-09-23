#pragma once

#include <string>
#include <vector>

namespace faaliha::faalihamart::util {

/**
 * @brief Input validation utility providing field-level checks before repository invocation.
 */
class ValidationUtil {
public:
    static void ValidateEmail(const std::string& email);
    static void ValidatePassword(const std::string& password);
    static void ValidateNonEmpty(const std::string& field_name, const std::string& value);
    static void ValidatePositive(const std::string& field_name, int64_t value);
    static void ValidateNonNegative(const std::string& field_name, int64_t value);
    static void ValidateRating(int32_t rating);
    static void ValidateRole(const std::string& role);
};

} // namespace faaliha::faalihamart::util
