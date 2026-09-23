#pragma once

#include <memory>
#include <string>
#include "../repository/IUserRepository.h"
#include "../dto/AuthDto.h"

namespace faaliha::faalihamart::service {

/**
 * @brief Handles user authentication, registration, and credentials verification.
 */
class AuthService {
public:
    /**
     * @brief Construct AuthService with injected user repository.
     * @param user_repo Shared pointer to user repository interface.
     */
    explicit AuthService(std::shared_ptr<repository::IUserRepository> user_repo);

    /**
     * @brief Register a new user with BUYER or SELLER role.
     * @param req Registration details.
     * @param request_id Contextual request ID for structured logging.
     * @return UserResponseDto without password hash.
     * @throws ValidationException on invalid input.
     * @throws ConflictException if email is already registered.
     */
    dto::UserResponseDto Register(const dto::RegisterRequestDto& req, const std::string& request_id = "");

    /**
     * @brief Authenticate a user by email and password.
     * @param req Login credentials.
     * @param request_id Contextual request ID for structured logging.
     * @return UserResponseDto on success.
     * @throws ValidationException on malformed input.
     * @throws UnauthorizedException on invalid credentials.
     */
    dto::UserResponseDto Login(const dto::LoginRequestDto& req, const std::string& request_id = "");

    /**
     * @brief Retrieve user profile by user ID.
     * @param user_id The user's ID.
     * @param request_id Contextual request ID.
     * @return UserResponseDto on success.
     * @throws NotFoundException if user does not exist.
     */
    dto::UserResponseDto GetUserProfile(int64_t user_id, const std::string& request_id = "");

private:
    std::shared_ptr<repository::IUserRepository> user_repo_;
};

} // namespace faaliha::faalihamart::service
