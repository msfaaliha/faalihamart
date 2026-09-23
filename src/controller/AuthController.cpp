#include "AuthController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../exception/ApiException.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::unique_ptr<service::AuthService> AuthController::GetAuthService() {
    sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
    auto user_repo = repository::RepositoryFactory::CreateUserRepository(db);
    return std::make_unique<service::AuthService>(user_repo);
}

void AuthController::Register(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        auto json_val = req->getJsonObject();
        if (!json_val) {
            throw exception::ValidationException("Request body must be valid JSON");
        }

        dto::RegisterRequestDto dto;
        dto.name = json_val->get("name", "").asString();
        dto.email = json_val->get("email", "").asString();
        dto.password = json_val->get("password", "").asString();
        dto.role = json_val->get("role", "BUYER").asString();

        auto auth_svc = GetAuthService();
        auto result = auth_svc->Register(dto, req_id);

        auto resp = CreateJsonResponse(dto::ApiResponse::Success(result.ToJson()), drogon::k201Created);
        callback(resp);
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void AuthController::Login(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        auto json_val = req->getJsonObject();
        if (!json_val) {
            throw exception::ValidationException("Request body must be valid JSON");
        }

        dto::LoginRequestDto dto;
        dto.email = json_val->get("email", "").asString();
        dto.password = json_val->get("password", "").asString();

        auto auth_svc = GetAuthService();
        auto user = auth_svc->Login(dto, req_id);

        // Section 5 Rule 3: Discard pre-auth session and issue a fresh one
        auto session = req->session();
        if (session) {
            session->clear();
            session->changeSessionIdToClient();
            session->insert("user_id", user.id);
            session->insert("role", user.role);
            session->insert("name", user.name);
            session->insert("email", user.email);
        }

        auto resp = CreateJsonResponse(dto::ApiResponse::Success(user.ToJson()), drogon::k200OK);
        callback(resp);
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void AuthController::Logout(const drogon::HttpRequestPtr& req,
                            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto session = req->session();
    if (session) {
        session->clear();
    }
    nlohmann::json data = {{"message", "Successfully logged out"}};
    callback(CreateJsonResponse(dto::ApiResponse::Success(data), drogon::k200OK));
}

void AuthController::GetCurrentUser(const drogon::HttpRequestPtr& req,
                                    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Not authenticated");
        }

        auto auth_svc = GetAuthService();
        auto user = auth_svc->GetUserProfile(user_id, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(user.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
