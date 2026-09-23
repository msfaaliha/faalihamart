#include "HealthController.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

void HealthController::CheckHealth(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
        auto repo = repository::RepositoryFactory::CreateMigrationRepository(db);
        bool db_up = repo->HealthCheck();

        nlohmann::json data = {
            {"status", "UP"},
            {"db", db_up ? "UP" : "DOWN"}
        };

        auto resp = CreateJsonResponse(data, db_up ? drogon::k200OK : drogon::k503ServiceUnavailable);
        callback(resp);
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
