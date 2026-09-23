#include "AdminController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../exception/ApiException.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::unique_ptr<service::AdminService> AdminController::GetAdminService() {
    sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
    auto user_repo = repository::RepositoryFactory::CreateUserRepository(db);
    auto prod_repo = repository::RepositoryFactory::CreateProductRepository(db);
    auto order_repo = repository::RepositoryFactory::CreateOrderRepository(db);
    return std::make_unique<service::AdminService>(user_repo, prod_repo, order_repo);
}

void AdminController::GetAllUsers(const drogon::HttpRequestPtr& req,
                                  std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        std::string role = GetCurrentUserRole(req);
        bool is_admin = (role == "ADMIN");
        if (!is_admin) {
            throw exception::ForbiddenException("Admin privileges required");
        }

        auto svc = GetAdminService();
        auto users = svc->GetAllUsers(is_admin, req_id);

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& u : users) {
            arr.push_back(u.ToJson());
        }

        callback(CreateJsonResponse(dto::ApiResponse::Success(arr), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void AdminController::GetAllOrders(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        std::string role = GetCurrentUserRole(req);
        bool is_admin = (role == "ADMIN");
        if (!is_admin) {
            throw exception::ForbiddenException("Admin privileges required");
        }

        auto svc = GetAdminService();
        auto orders = svc->GetAllOrders(is_admin, req_id);

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& o : orders) {
            arr.push_back(o.ToJson());
        }

        callback(CreateJsonResponse(dto::ApiResponse::Success(arr), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void AdminController::GetDashboardStats(const drogon::HttpRequestPtr& req,
                                        std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        std::string role = GetCurrentUserRole(req);
        bool is_admin = (role == "ADMIN");
        if (!is_admin) {
            throw exception::ForbiddenException("Admin privileges required");
        }

        auto svc = GetAdminService();
        auto stats = svc->GetDashboardStats(is_admin, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(stats.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
