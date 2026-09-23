#include "AuthService.h"
#include "../util/ValidationUtil.h"
#include "../util/PasswordUtil.h"
#include "../exception/ApiException.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::service {

AuthService::AuthService(std::shared_ptr<repository::IUserRepository> user_repo)
    : user_repo_(std::move(user_repo)) {}

dto::UserResponseDto AuthService::Register(const dto::RegisterRequestDto& req, const std::string& request_id) {
    spdlog::info("[{}] AuthService::Register: attempting registration for email={}", request_id, req.email);

    // 1. Validation at top of service method
    util::ValidationUtil::ValidateNonEmpty("name", req.name);
    util::ValidationUtil::ValidateEmail(req.email);
    util::ValidationUtil::ValidatePassword(req.password);
    util::ValidationUtil::ValidateRole(req.role);

    // 2. Check for duplicate email
    if (user_repo_->FindByEmail(req.email).has_value()) {
        spdlog::warn("[{}] AuthService::Register: email {} already exists", request_id, req.email);
        throw exception::ConflictException("An account with this email address already exists");
    }

    // 3. Hash password via Argon2id
    std::string hash = util::PasswordUtil::HashPassword(req.password);

    // 4. Create and persist user entity
    model::User user;
    user.name_ = req.name;
    user.email_ = req.email;
    user.password_hash_ = hash;
    user.role_ = model::StringToRole(req.role);

    model::User created = user_repo_->Create(user);
    spdlog::info("[{}] AuthService::Register: successfully registered user_id={}, role={}",
                 request_id, created.id_, model::RoleToString(created.role_));

    return dto::UserResponseDto::FromEntity(created);
}

dto::UserResponseDto AuthService::Login(const dto::LoginRequestDto& req, const std::string& request_id) {
    spdlog::info("[{}] AuthService::Login: attempting login for email={}", request_id, req.email);

    // 1. Validate inputs
    util::ValidationUtil::ValidateEmail(req.email);
    util::ValidationUtil::ValidateNonEmpty("password", req.password);

    // 2. Find user
    auto user_opt = user_repo_->FindByEmail(req.email);
    if (!user_opt.has_value()) {
        spdlog::warn("[{}] AuthService::Login: user not found with email={}", request_id, req.email);
        throw exception::UnauthorizedException("Invalid email or password");
    }

    // 3. Verify password hash
    if (!util::PasswordUtil::VerifyPassword(user_opt->password_hash_, req.password)) {
        spdlog::warn("[{}] AuthService::Login: invalid password attempt for email={}", request_id, req.email);
        throw exception::UnauthorizedException("Invalid email or password");
    }

    spdlog::info("[{}] AuthService::Login: login successful for user_id={}, role={}",
                 request_id, user_opt->id_, model::RoleToString(user_opt->role_));

    return dto::UserResponseDto::FromEntity(*user_opt);
}

dto::UserResponseDto AuthService::GetUserProfile(int64_t user_id, const std::string& request_id) {
    spdlog::info("[{}] AuthService::GetUserProfile: user_id={}", request_id, user_id);

    auto user_opt = user_repo_->FindById(user_id);
    if (!user_opt.has_value()) {
        throw exception::NotFoundException("User profile not found");
    }

    return dto::UserResponseDto::FromEntity(*user_opt);
}

} // namespace faaliha::faalihamart::service
