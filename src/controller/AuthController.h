#pragma once

#include "BaseController.h"
#include "../service/AuthService.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class AuthController : public drogon::HttpController<AuthController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::Register, "/api/v1/auth/register", drogon::Post);
    ADD_METHOD_TO(AuthController::Login, "/api/v1/auth/login", drogon::Post);
    ADD_METHOD_TO(AuthController::Logout, "/api/v1/auth/logout", drogon::Post);
    ADD_METHOD_TO(AuthController::GetCurrentUser, "/api/v1/auth/me", drogon::Get);
    METHOD_LIST_END

    void Register(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void Login(const drogon::HttpRequestPtr& req,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void Logout(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetCurrentUser(const drogon::HttpRequestPtr& req,
                        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::unique_ptr<service::AuthService> GetAuthService();
};

} // namespace faaliha::faalihamart::controller
#endif
