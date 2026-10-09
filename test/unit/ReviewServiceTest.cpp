#include <gtest/gtest.h>
#include <unordered_map>
#include "service/ReviewService.h"
#include "repository/IReviewRepository.h"
#include "repository/IOrderRepository.h"
#include "exception/ApiException.h"

using namespace faaliha::faalihamart;

class FakeReviewRepository : public repository::IReviewRepository {
public:
    std::vector<model::Review> FindByProductId(int64_t product_id) override {
        std::vector<model::Review> res;
        for (const auto& [_, r] : reviews_) {
            if (r.product_id_ == product_id) res.push_back(r);
        }
        return res;
    }

    std::optional<model::Review> FindById(int64_t id) override {
        auto it = reviews_.find(id);
        if (it != reviews_.end()) return it->second;
        return std::nullopt;
    }

    model::Review Create(const model::Review& review) override {
        model::Review r = review;
        r.id_ = next_id_++;
        reviews_[r.id_] = r;
        return r;
    }

    bool Delete(int64_t id) override {
        return reviews_.erase(id) > 0;
    }

    bool HasUserPurchasedAndDelivered(int64_t user_id, int64_t product_id) override {
        return (user_id == 4 && product_id == 1);
    }

    std::pair<double, int64_t> GetAverageRatingAndCount(int64_t /*product_id*/) override {
        return {4.5, 2};
    }

private:
    int64_t next_id_{1};
    std::unordered_map<int64_t, model::Review> reviews_;
};

class DummyOrderRepoForReview : public repository::IOrderRepository {
public:
    std::optional<model::Order> FindById(int64_t) override { return std::nullopt; }
    std::vector<model::Order> FindByBuyerId(int64_t) override { return {}; }
    std::vector<model::Order> FindBySellerId(int64_t) override { return {}; }
    std::vector<model::Order> FindAll() override { return {}; }
    model::Order CreateOrderWithItems(int64_t, const std::vector<model::CartItem>&, const model::Money&) override { return {}; }
    bool UpdateStatus(int64_t, model::OrderStatus) override { return false; }
    int64_t Count() override { return 0; }
    model::Money CalculateTotalPlatformRevenue() override { return model::Money::FromCents(0); }
    model::Money CalculateSellerRevenue(int64_t) override { return model::Money::FromCents(0); }
    int64_t CountSellerOrders(int64_t) override { return 0; }
};

TEST(ReviewServiceTest, ValidatesRatingRange) {
    auto review_repo = std::make_shared<FakeReviewRepository>();
    auto order_repo = std::make_shared<DummyOrderRepoForReview>();
    service::ReviewService review_svc(review_repo, order_repo);

    dto::CreateReviewRequestDto invalid_high;
    invalid_high.product_id = 1;
    invalid_high.rating = 6; // Out of range (>5)
    invalid_high.comment = "Super product!";

    EXPECT_THROW(
        review_svc.AddReview(4, invalid_high, "req-rev-1"),
        exception::ValidationException
    );

    dto::CreateReviewRequestDto invalid_low;
    invalid_low.product_id = 1;
    invalid_low.rating = 0; // Out of range (<1)
    invalid_low.comment = "Poor";

    EXPECT_THROW(
        review_svc.AddReview(4, invalid_low, "req-rev-2"),
        exception::ValidationException
    );
}

TEST(ReviewServiceTest, RequiresVerifiedDeliveredPurchase) {
    auto review_repo = std::make_shared<FakeReviewRepository>();
    auto order_repo = std::make_shared<DummyOrderRepoForReview>();
    service::ReviewService review_svc(review_repo, order_repo);

    // User 99 has NOT purchased product 1
    dto::CreateReviewRequestDto req;
    req.product_id = 1;
    req.rating = 5;
    req.comment = "Attempting unverified review";

    EXPECT_THROW(
        review_svc.AddReview(99, req, "req-rev-3"),
        exception::ForbiddenException
    );

    // User 4 HAS purchased and had it delivered
    auto created = review_svc.AddReview(4, req, "req-rev-4");
    EXPECT_EQ(created.rating, 5);
    EXPECT_EQ(created.product_id, 1);
}

TEST(ReviewServiceTest, AdminCanModerateReview) {
    auto review_repo = std::make_shared<FakeReviewRepository>();
    auto order_repo = std::make_shared<DummyOrderRepoForReview>();
    service::ReviewService review_svc(review_repo, order_repo);

    // Non-admin cannot delete
    EXPECT_THROW(
        review_svc.DeleteReview(1, false, "req-rev-5"),
        exception::ForbiddenException
    );

    // Admin can delete
    EXPECT_NO_THROW(
        review_svc.DeleteReview(1, true, "req-rev-6")
    );
}
