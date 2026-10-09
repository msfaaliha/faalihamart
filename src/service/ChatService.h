#pragma once

#include "chat/ChatProvider.h"
#include "../repository/IProductRepository.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <deque>
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>

namespace faaliha::faalihamart::service {

struct ChatResult {
    std::string reply;
    std::string provider;
    bool cached{false};

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"reply", reply},
            {"provider", provider},
            {"cached", cached}
        };
    }
};

/**
 * @brief Service coordinating AI chatbot requests, enforcing per-session rate limits,
 * in-memory session caching, and domain context enrichment (Sections 9, 15).
 */
class ChatService {
public:
    explicit ChatService(std::shared_ptr<chat::ChatProvider> provider,
                         std::shared_ptr<repository::IProductRepository> product_repo = nullptr);

    /**
     * @brief Processes a user chat message with rate limiting and caching.
     * @param session_id Drogon cookie-backed session identifier.
     * @param message User's input message.
     * @param request_id Unique request ID for structured logging.
     * @return ChatResult containing reply, provider name, and cached flag.
     */
    ChatResult ProcessMessage(const std::string& session_id,
                              const std::string& message,
                              const std::string& request_id);

    /**
     * @brief Clears in-memory session cache (useful for testing).
     */
    void ClearSessionCache(const std::string& session_id);

    /**
     * @brief Checks whether the session has exceeded the 10 messages/minute limit.
     */
    bool CheckRateLimit(const std::string& session_id);

    /**
     * @brief Resets rate limits for testing.
     */
    void ResetRateLimits();

    [[nodiscard]] std::string GetProviderName() const {
        return provider_ ? provider_->GetProviderName() : "none";
    }

private:
    std::string BuildCatalogContext();

    std::shared_ptr<chat::ChatProvider> provider_;
    std::shared_ptr<repository::IProductRepository> product_repo_;

    // Per-session rate limiter: 10 messages/minute (Section 9 Rule 4)
    std::mutex rate_limit_mutex_;
    std::unordered_map<std::string, std::deque<std::chrono::steady_clock::time_point>> rate_limit_tracker_;

    // Per-session identical question cache: session_id -> (question -> reply) (Section 9 Rule 5)
    std::mutex cache_mutex_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> session_cache_;
};

} // namespace faaliha::faalihamart::service
