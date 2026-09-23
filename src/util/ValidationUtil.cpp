#include "ValidationUtil.h"
#include "../exception/ApiException.h"
#include <regex>
#include <cctype>

namespace faaliha::faalihamart::util {

void ValidationUtil::ValidateEmail(const std::string& email) {
    if (email.empty()) {
        throw exception::ValidationException("Email cannot be empty", {"email: required"});
    }
    const std::regex email_regex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
    if (!std::regex_match(email, email_regex)) {
        throw exception::ValidationException("Invalid email format", {"email: must be a valid email address"});
    }
}

void ValidationUtil::ValidatePassword(const std::string& password) {
    if (password.length() < 8) {
        throw exception::ValidationException("Password too short", {"password: must be at least 8 characters"});
    }
    bool has_alpha = false;
    bool has_digit = false;
    for (char c : password) {
        if (std::isalpha(static_cast<unsigned char>(c))) has_alpha = true;
        if (std::isdigit(static_cast<unsigned char>(c))) has_digit = true;
    }
    if (!has_alpha || !has_digit) {
        throw exception::ValidationException("Password too weak", {"password: must contain at least one letter and one number"});
    }
}

void ValidationUtil::ValidateNonEmpty(const std::string& field_name, const std::string& value) {
    if (value.empty() || value.find_first_not_of(" \t\n\r") == std::string::npos) {
        throw exception::ValidationException("Field cannot be empty", {field_name + ": cannot be blank"});
    }
}

void ValidationUtil::ValidatePositive(const std::string& field_name, int64_t value) {
    if (value <= 0) {
        throw exception::ValidationException("Value must be strictly positive", {field_name + ": must be greater than zero"});
    }
}

void ValidationUtil::ValidateNonNegative(const std::string& field_name, int64_t value) {
    if (value < 0) {
        throw exception::ValidationException("Value cannot be negative", {field_name + ": must be greater than or equal to zero"});
    }
}

void ValidationUtil::ValidateRating(int32_t rating) {
    if (rating < 1 || rating > 5) {
        throw exception::ValidationException("Invalid rating score", {"rating: must be an integer between 1 and 5"});
    }
}

void ValidationUtil::ValidateRole(const std::string& role) {
    if (role != "BUYER" && role != "SELLER") {
        throw exception::ValidationException("Invalid registration role", {"role: must be either BUYER or SELLER"});
    }
}

} // namespace faaliha::faalihamart::util
