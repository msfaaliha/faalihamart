#pragma once

#include "BaseController.h"
#include "../plugin/DbPlugin.h"
#include "../service/MigrationService.h"
#include "../repository/RepositoryFactory.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class HealthController : public drogon::HttpController<HealthController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HealthController::CheckHealth, "/api/v1/health", drogon::Get);
    METHOD_LIST_END

    void CheckHealth(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace faaliha::faalihamart::controller
#endif
