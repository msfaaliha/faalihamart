#include "ChatController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../service/chat/MockChatProvider.h"
#include "../service/chat/GeminiChatProvider.h"
#include "../exception/ApiException.h"
#include <cstdlib>

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::shared_ptr<service::ChatService> ChatController::GetChatService() {
    static std::shared_ptr<service::ChatService> s_chat_service = []() {
        std::string provider_type = "mock";

        // 1. Check environment variable first
        const char* env_prov = std::getenv("AI_CHAT_PROVIDER");
        if (env_prov) {
            provider_type = std::string(env_prov);
        } else {
            // 2. Check Drogon custom config
            try {
                auto custom = drogon::app().getCustomConfig();
                if (custom.isMember("ai") && custom["ai"].isMember("chatbot")) {
                    provider_type = custom["ai"]["chatbot"].get("provider", "mock").asString();
                }
            } catch (...) {}
        }

        std::shared_ptr<service::chat::ChatProvider> provider;
        if (provider_type == "gemini") {
            provider = std::make_shared<service::chat::GeminiChatProvider>();
        } else {
            provider = std::make_shared<service::chat::MockChatProvider>();
        }

        sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
        auto prod_repo = repository::RepositoryFactory::CreateProductRepository(db);

        return std::make_shared<service::ChatService>(provider, prod_repo);
    }();

    return s_chat_service;
}

void ChatController::SendMessage(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        auto json_val = req->getJsonObject();
        if (!json_val || !json_val->isMember("message")) {
            throw exception::ValidationException("Request body must be valid JSON containing 'message'");
        }

        std::string message = json_val->get("message", "").asString();

        // Extract session ID from cookie-backed Drogon session (Section 9 Rule 4)
        std::string session_id = "anon";
        auto session = req->session();
        if (session) {
            session_id = session->sessionId();
        }

        auto chat_svc = GetChatService();
        auto result = chat_svc->ProcessMessage(session_id, message, req_id);

        auto resp = CreateJsonResponse(dto::ApiResponse::Success(result.ToJson()), drogon::k200OK);
        callback(resp);
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void ChatController::GetProviderStatus(const drogon::HttpRequestPtr& req,
                                       std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        auto chat_svc = GetChatService();
        nlohmann::json data = {
            {"provider", chat_svc->GetProviderName()},
            {"rate_limit_per_minute", 10},
            {"max_message_length", 500}
        };
        callback(CreateJsonResponse(dto::ApiResponse::Success(data), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
