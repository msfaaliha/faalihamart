#include <gtest/gtest.h>
#include "service/ChatService.h"
#include "service/chat/MockChatProvider.h"
#include "exception/ApiException.h"
#include <memory>

using namespace faaliha::faalihamart;

class ChatServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto provider = std::make_shared<service::chat::MockChatProvider>();
        chat_service_ = std::make_unique<service::ChatService>(provider, nullptr);
    }

    std::unique_ptr<service::ChatService> chat_service_;
};

TEST_F(ChatServiceTest, ReturnsDomainFaqReply) {
    auto res = chat_service_->ProcessMessage("session-1", "What headphones do you have?", "req-test-1");
    EXPECT_FALSE(res.reply.empty());
    EXPECT_EQ(res.provider, "mock");
    EXPECT_FALSE(res.cached);
    EXPECT_NE(res.reply.find("Noise-Cancelling Wireless Headphones"), std::string::npos);
}

TEST_F(ChatServiceTest, RejectsEmptyMessage) {
    EXPECT_THROW(
        chat_service_->ProcessMessage("session-1", "   ", "req-test-2"),
        exception::ValidationException
    );
}

TEST_F(ChatServiceTest, RejectsExcessivelyLongMessage) {
    std::string long_msg(501, 'a');
    EXPECT_THROW(
        chat_service_->ProcessMessage("session-1", long_msg, "req-test-3"),
        exception::ValidationException
    );
}

TEST_F(ChatServiceTest, CachesRepeatedIdenticalQuestionWithinSession) {
    std::string q = "What is your return policy?";
    auto res1 = chat_service_->ProcessMessage("session-cache-1", q, "req-test-4a");
    EXPECT_FALSE(res1.cached);

    auto res2 = chat_service_->ProcessMessage("session-cache-1", q, "req-test-4b");
    EXPECT_TRUE(res2.cached);
    EXPECT_EQ(res1.reply, res2.reply);

    // Different session ID should not serve cache
    auto res3 = chat_service_->ProcessMessage("session-cache-2", q, "req-test-4c");
    EXPECT_FALSE(res3.cached);
}

TEST_F(ChatServiceTest, EnforcesRateLimitTenMessagesPerMinute) {
    std::string session_id = "session-rate-limit";
    for (int i = 1; i <= 10; ++i) {
        EXPECT_NO_THROW(
            chat_service_->ProcessMessage(session_id, "Question #" + std::to_string(i), "req-limit-" + std::to_string(i))
        );
    }

    // 11th message should throw rate limit exception
    EXPECT_THROW(
        chat_service_->ProcessMessage(session_id, "Question #11", "req-limit-11"),
        exception::ApiException
    );
}
