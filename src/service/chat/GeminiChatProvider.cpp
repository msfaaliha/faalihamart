#include "GeminiChatProvider.h"
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#if __has_include(<drogon/drogon.h>)
#include <drogon/drogon.h>
#define HAS_DROGON_CLIENT 1
#else
#define HAS_DROGON_CLIENT 0
#endif

namespace faaliha::faalihamart::service::chat {

GeminiChatProvider::GeminiChatProvider(std::string api_key,
                                       std::string model_name,
                                       int timeout_seconds)
    : api_key_(std::move(api_key)),
      model_name_(std::move(model_name)),
      timeout_seconds_(timeout_seconds),
      fallback_provider_(std::make_unique<MockChatProvider>()) {
    if (api_key_.empty()) {
        const char* env_key = std::getenv("GEMINI_API_KEY");
        if (env_key) {
            api_key_ = std::string(env_key);
        }
    }
}

std::string GeminiChatProvider::BuildRequestBody(const std::string& user_message,
                                                 const std::string& context) const {
    std::string system_instruction =
        "You are the official FaalihaMart AI Shopping & Support Assistant. "
        "FaalihaMart is a multi-seller e-commerce marketplace featuring Electronics, Apparel, and Home & Living products with prices in INR (₹). "
        "Orders follow a lifecycle: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED. "
        "Checkout uses simulated mock payment verification. Reviews (1-5 stars) require a delivered order. "
        "Always be concise, friendly, and helpful. Confine your answers strictly to the FaalihaMart marketplace domain. ";

    if (!context.empty()) {
        system_instruction += "Context:\n" + context + "\n";
    }

    nlohmann::json payload;
    payload["system_instruction"]["parts"] = nlohmann::json::array({
        {{"text", system_instruction}}
    });
    payload["contents"] = nlohmann::json::array({
        {
            {"role", "user"},
            {"parts", nlohmann::json::array({{{"text", user_message}}})}
        }
    });

    nlohmann::json gen_config;
    gen_config["temperature"] = 0.3;
    gen_config["maxOutputTokens"] = 300;
    payload["generationConfig"] = gen_config;

    return payload.dump();
}

std::string GeminiChatProvider::ParseResponseBody(const std::string& response_json) const {
    try {
        auto parsed = nlohmann::json::parse(response_json);
        if (parsed.contains("candidates") && parsed["candidates"].is_array() && !parsed["candidates"].empty()) {
            auto first_candidate = parsed["candidates"][0];
            if (first_candidate.contains("content") && first_candidate["content"].contains("parts")) {
                auto parts = first_candidate["content"]["parts"];
                if (parts.is_array() && !parts.empty() && parts[0].contains("text")) {
                    return parts[0]["text"].get<std::string>();
                }
            }
        }
    } catch (const std::exception& e) {
        spdlog::warn("Failed to parse Gemini API response: {}", e.what());
    }
    return "";
}

std::string GeminiChatProvider::GetReply(const std::string& user_message,
                                         const std::string& context) {
    if (api_key_.empty()) {
        spdlog::debug("GEMINI_API_KEY not configured. Using MockChatProvider fallback.");
        return fallback_provider_->GetReply(user_message, context);
    }

#if HAS_DROGON_CLIENT
    try {
        auto client = drogon::HttpClient::newHttpClient("https://generativelanguage.googleapis.com");
        auto req = drogon::HttpRequest::newHttpRequest();
        std::string path = "/v1beta/models/" + model_name_ + ":generateContent?key=" + api_key_;
        req->setPath(path);
        req->setMethod(drogon::Post);
        req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        req->setBody(BuildRequestBody(user_message, context));

        auto [result, resp] = client->sendRequest(req, static_cast<double>(timeout_seconds_));
        if (result == drogon::ReqResult::Ok && resp && resp->getStatusCode() == drogon::k200OK) {
            std::string text = ParseResponseBody(std::string(resp->getBody()));
            if (!text.empty()) {
                return text;
            }
        } else {
            spdlog::warn("Gemini HTTP call failed (result: {}, status: {}). Falling back to mock response.",
                         static_cast<int>(result), resp ? static_cast<int>(resp->getStatusCode()) : 0);
        }
    } catch (const std::exception& ex) {
        spdlog::error("Exception in GeminiChatProvider::GetReply: {}. Falling back.", ex.what());
    }
#else
    spdlog::warn("Drogon HTTP client unavailable in current build. Using mock provider fallback.");
#endif

    // Section 9 Rule 3: On failure or timeout, return a static degraded response
    return fallback_provider_->GetReply(user_message, context);
}

} // namespace faaliha::faalihamart::service::chat
