#pragma once

#include <string>

namespace faaliha::faalihamart::service::chat {

/**
 * @brief Abstract base interface for AI chatbot response providers (Section 15).
 */
class ChatProvider {
public:
    virtual ~ChatProvider() = default;

    /**
     * @brief Generates an AI or FAQ response for a given user query.
     * @param user_message The incoming user message.
     * @param context Domain context including catalog overview and store policies.
     * @return Generated reply string.
     */
    virtual std::string GetReply(const std::string& user_message,
                                 const std::string& context) = 0;

    /**
     * @brief Returns the provider identifier (e.g., 'mock' or 'gemini').
     */
    [[nodiscard]] virtual std::string GetProviderName() const = 0;
};

} // namespace faaliha::faalihamart::service::chat
