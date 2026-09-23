#pragma once

#include "BaseController.h"
#include "../service/AdminService.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class AdminController : public drogon::HttpController<AdminController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AdminController::GetAllUsers, "/api/v1/admin/users", drogon::Get);
    ADD_METHOD_TO(AdminController::GetAllOrders, "/api/v1/admin/orders", drogon::Get);
    ADD_METHOD_TO(AdminController::GetDashboardStats, "/api/v1/admin/stats", drogon::Get);
    METHOD_LIST_END

    void GetAllUsers(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetAllOrders(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetDashboardStats(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::unique_ptr<service::AdminService> GetAdminService();
};

} // namespace faaliha::faalihamart::controller
#endif
