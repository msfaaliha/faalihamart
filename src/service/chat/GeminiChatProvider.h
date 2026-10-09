#pragma once

#include "ChatProvider.h"
#include "MockChatProvider.h"
#include <string>
#include <memory>

namespace faaliha::faalihamart::service::chat {

/**
 * @brief Live Gemini AI provider communicating with the Google Gemini REST API (Section 9 & 15).
 * Reads GEMINI_API_KEY server-side from environment variables.
 * Falls back safely to MockChatProvider or degraded static message on timeout/network failure.
 */
class GeminiChatProvider : public ChatProvider {
public:
    explicit GeminiChatProvider(std::string api_key = "",
                                std::string model_name = "gemini-1.5-flash",
                                int timeout_seconds = 10);
    ~GeminiChatProvider() override = default;

    std::string GetReply(const std::string& user_message,
                         const std::string& context) override;

    [[nodiscard]] std::string GetProviderName() const override {
        return "gemini";
    }

    [[nodiscard]] bool HasApiKey() const {
        return !api_key_.empty();
    }

private:
    std::string BuildRequestBody(const std::string& user_message, const std::string& context) const;
    std::string ParseResponseBody(const std::string& response_json) const;

    std::string api_key_;
    std::string model_name_;
    int timeout_seconds_{10};
    std::unique_ptr<MockChatProvider> fallback_provider_;
};

} // namespace faaliha::faalihamart::service::chat
