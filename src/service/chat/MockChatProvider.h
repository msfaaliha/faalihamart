#pragma once

#include "ChatProvider.h"
#include <string>
#include <vector>
#include <utility>

namespace faaliha::faalihamart::service::chat {

/**
 * @brief Mock AI provider supplying canned FAQ answers for 5-10 domain questions (Section 9 & 15).
 * Operates offline without network dependencies.
 */
class MockChatProvider : public ChatProvider {
public:
    MockChatProvider();
    ~MockChatProvider() override = default;

    std::string GetReply(const std::string& user_message,
                         const std::string& context) override;

    [[nodiscard]] std::string GetProviderName() const override {
        return "mock";
    }

private:
    struct FaqEntry {
        std::vector<std::string> keywords;
        std::string answer;
    };

    std::vector<FaqEntry> faq_database_;
};

} // namespace faaliha::faalihamart::service::chat
