#pragma once

#include "BaseController.h"
#include "../service/ChatService.h"
#include <memory>

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

/**
 * @brief HTTP Controller exposing AI Chatbot endpoints (Section 9, 15).
 */
class ChatController : public drogon::HttpController<ChatController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ChatController::SendMessage, "/api/v1/chat", drogon::Post);
    ADD_METHOD_TO(ChatController::SendMessage, "/api/chat", drogon::Post);
    ADD_METHOD_TO(ChatController::GetProviderStatus, "/api/v1/chat/provider", drogon::Get);
    METHOD_LIST_END

    void SendMessage(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetProviderStatus(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    static std::shared_ptr<service::ChatService> GetChatService();
};

} // namespace faaliha::faalihamart::controller
#endif
