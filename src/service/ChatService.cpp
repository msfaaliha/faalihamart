#include "ChatService.h"
#include "../exception/ApiException.h"
#include "../util/ValidationUtil.h"
#include <spdlog/spdlog.h>
#include <sstream>

namespace faaliha::faalihamart::service {

namespace {
constexpr size_t kMaxMessageLength = 500;
constexpr size_t kRateLimitMaxMessages = 10;
constexpr auto kRateLimitWindow = std::chrono::seconds(60);
} // namespace

ChatService::ChatService(std::shared_ptr<chat::ChatProvider> provider,
                         std::shared_ptr<repository::IProductRepository> product_repo)
    : provider_(std::move(provider)),
      product_repo_(std::move(product_repo)) {}

bool ChatService::CheckRateLimit(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    auto now = std::chrono::steady_clock::now();
    auto& timestamps = rate_limit_tracker_[session_id];

    // Remove entries older than 60 seconds
    while (!timestamps.empty() && (now - timestamps.front()) > kRateLimitWindow) {
        timestamps.pop_front();
    }

    if (timestamps.size() >= kRateLimitMaxMessages) {
        return false; // Exceeded
    }

    timestamps.push_back(now);
    return true;
}

void ChatService::ResetRateLimits() {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    rate_limit_tracker_.clear();
}

void ChatService::ClearSessionCache(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    session_cache_.erase(session_id);
}

std::string ChatService::BuildCatalogContext() {
    std::ostringstream oss;
    oss << "Available categories: Electronics, Apparel, Home & Living.\n";

    if (product_repo_) {
        dto::ProductFilterDto filter;
        auto prods = product_repo_->FindAll(filter);
        oss << "Current featured products in store:\n";
        size_t count = 0;
        for (const auto& p : prods) {
            oss << "- " << p.name_ << " (" << p.category_ << ", "
                << p.price_cents_.ToString() << ", Stock: " << p.stock_qty_ << ")\n";
            if (++count >= 10) break;
        }
    }
    return oss.str();
}

ChatResult ChatService::ProcessMessage(const std::string& session_id,
                                       const std::string& message,
                                       const std::string& request_id) {
    spdlog::info("[{}] ChatService::ProcessMessage: session_id='{}', length={}",
                 request_id, session_id, message.length());

    // 1. Input Validation (Section 9 Rule 3)
    util::ValidationUtil::ValidateNonEmpty("message", message);
    if (message.length() > kMaxMessageLength) {
        throw exception::ValidationException(
            "Message length exceeds maximum allowed limit",
            {"message: cannot exceed 500 characters"}
        );
    }

    std::string safe_session_id = session_id.empty() ? "anon_session" : session_id;

    // 2. Per-Session Rate Limiting: 10 msg/min (Section 9 Rule 4)
    if (!CheckRateLimit(safe_session_id)) {
        spdlog::warn("[{}] Rate limit exceeded for session '{}'", request_id, safe_session_id);
        throw exception::ApiException(
            "RATE_LIMIT_EXCEEDED",
            "Rate limit exceeded. Maximum 10 messages per minute allowed per session.",
            429
        );
    }

    // 3. Repeated Identical Question Cache (Section 9 Rule 5)
    {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        auto session_it = session_cache_.find(safe_session_id);
        if (session_it != session_cache_.end()) {
            auto hit_it = session_it->second.find(message);
            if (hit_it != session_it->second.end()) {
                spdlog::info("[{}] Serving cached reply for session '{}'", request_id, safe_session_id);
                return ChatResult{
                    hit_it->second,
                    provider_ ? provider_->GetProviderName() : "mock",
                    true
                };
            }
        }
    }

    // 4. Retrieve Answer from Provider
    std::string context = BuildCatalogContext();
    std::string reply;
    if (provider_) {
        reply = provider_->GetReply(message, context);
    } else {
        reply = "FaalihaMart Assistant is temporarily in degraded mode. Please browse products above.";
    }

    // 5. Store in Session Cache
    {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        session_cache_[safe_session_id][message] = reply;
    }

    return ChatResult{
        reply,
        provider_ ? provider_->GetProviderName() : "mock",
        false
    };
}

} // namespace faaliha::faalihamart::service
