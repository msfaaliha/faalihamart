#pragma once

#include "BaseController.h"
#include "../service/AdminService.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class SellerController : public drogon::HttpController<SellerController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SellerController::GetDashboardStats, "/api/v1/seller/stats", drogon::Get);
    METHOD_LIST_END

    void GetDashboardStats(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::unique_ptr<service::AdminService> GetAdminService();
};

} // namespace faaliha::faalihamart::controller
#endif
