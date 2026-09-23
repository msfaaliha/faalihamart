#include "SellerController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../exception/ApiException.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::unique_ptr<service::AdminService> SellerController::GetAdminService() {
    sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
    auto user_repo = repository::RepositoryFactory::CreateUserRepository(db);
    auto prod_repo = repository::RepositoryFactory::CreateProductRepository(db);
    auto order_repo = repository::RepositoryFactory::CreateOrderRepository(db);
    return std::make_unique<service::AdminService>(user_repo, prod_repo, order_repo);
}

void SellerController::GetDashboardStats(const drogon::HttpRequestPtr& req,
                                         std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        std::string role = GetCurrentUserRole(req);
        if (user_id <= 0 || (role != "SELLER" && role != "ADMIN")) {
            throw exception::ForbiddenException("Seller access required");
        }

        auto svc = GetAdminService();
        auto stats = svc->GetSellerDashboardStats(user_id, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(stats.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
