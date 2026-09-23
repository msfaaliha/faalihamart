#pragma once

#include <string>
#include <vector>
#include <stdexcept>

namespace faaliha::faalihamart::exception {

/**
 * @brief Base API exception mapped to HTTP status code and fixed error code.
 */
class ApiException : public std::runtime_error {
public:
    ApiException(int http_status, std::string code, std::string message, std::vector<std::string> details = {})
        : std::runtime_error(message),
          http_status_(http_status),
          code_(std::move(code)),
          message_(std::move(message)),
          details_(std::move(details)) {}

    [[nodiscard]] int GetHttpStatus() const noexcept { return http_status_; }
    [[nodiscard]] const std::string& GetCode() const noexcept { return code_; }
    [[nodiscard]] const std::string& GetApiMessage() const noexcept { return message_; }
    [[nodiscard]] const std::vector<std::string>& GetDetails() const noexcept { return details_; }

private:
    int http_status_{500};
    std::string code_;
    std::string message_;
    std::vector<std::string> details_;
};

class ValidationException : public ApiException {
public:
    explicit ValidationException(std::string message, std::vector<std::string> details = {})
        : ApiException(400, "VALIDATION_ERROR", std::move(message), std::move(details)) {}
};

class UnauthorizedException : public ApiException {
public:
    explicit UnauthorizedException(std::string message = "Authentication required")
        : ApiException(401, "UNAUTHORIZED", std::move(message)) {}
};

class ForbiddenException : public ApiException {
public:
    explicit ForbiddenException(std::string message = "Access denied: insufficient permissions")
        : ApiException(403, "FORBIDDEN", std::move(message)) {}
};

class NotFoundException : public ApiException {
public:
    explicit NotFoundException(std::string message = "Requested resource not found")
        : ApiException(404, "NOT_FOUND", std::move(message)) {}
};

class ConflictException : public ApiException {
public:
    explicit ConflictException(std::string message)
        : ApiException(409, "CONFLICT", std::move(message)) {}
};

class InternalServerErrorException : public ApiException {
public:
    explicit InternalServerErrorException(std::string message = "Internal server error")
        : ApiException(500, "INTERNAL_ERROR", std::move(message)) {}
};

} // namespace faaliha::faalihamart::exception
