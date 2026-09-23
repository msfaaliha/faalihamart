#include "ReviewService.h"
#include "../util/ValidationUtil.h"
#include "../exception/ApiException.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::service {

ReviewService::ReviewService(std::shared_ptr<repository::IReviewRepository> review_repo,
                             std::shared_ptr<repository::IOrderRepository> order_repo)
    : review_repo_(std::move(review_repo)), order_repo_(std::move(order_repo)) {}

std::vector<dto::ReviewResponseDto> ReviewService::GetProductReviews(int64_t product_id, const std::string& request_id) {
    spdlog::info("[{}] ReviewService::GetProductReviews: product_id={}", request_id, product_id);
    auto reviews = review_repo_->FindByProductId(product_id);
    std::vector<dto::ReviewResponseDto> dtos;
    dtos.reserve(reviews.size());
    for (const auto& r : reviews) {
        dtos.push_back(dto::ReviewResponseDto::FromEntity(r));
    }
    return dtos;
}

dto::ReviewResponseDto ReviewService::AddReview(int64_t user_id, const dto::CreateReviewRequestDto& req, const std::string& request_id) {
    spdlog::info("[{}] ReviewService::AddReview: user_id={}, product_id={}, rating={}",
                 request_id, user_id, req.product_id, req.rating);

    // 1. Validation at top of service method
    util::ValidationUtil::ValidatePositive("product_id", req.product_id);
    util::ValidationUtil::ValidateRating(req.rating);
    util::ValidationUtil::ValidateNonEmpty("comment", req.comment);

    // 2. Enforce F8 requirement: Reviews allowed only on completed (DELIVERED) orders
    bool verified_purchase = order_repo_->HasDeliveredPurchase(user_id, req.product_id);
    if (!verified_purchase) {
        spdlog::warn("[{}] User {} attempted to review product {} without a delivered order",
                     request_id, user_id, req.product_id);
        throw exception::ForbiddenException("You can only review products from completed, delivered orders");
    }

    // 3. Prevent duplicate reviews by same user for same product
    auto existing_review = review_repo_->FindByUserAndProduct(user_id, req.product_id);
    if (existing_review.has_value()) {
        throw exception::ConflictException("You have already reviewed this product");
    }

    model::Review review;
    review.product_id_ = req.product_id;
    review.user_id_ = user_id;
    review.rating_ = req.rating;
    review.comment_ = req.comment;

    model::Review created = review_repo_->Create(review);
    auto created_with_user = review_repo_->FindByUserAndProduct(user_id, req.product_id);
    return dto::ReviewResponseDto::FromEntity(created_with_user.value_or(created));
}

bool ReviewService::DeleteReview(int64_t review_id, bool is_admin, const std::string& request_id) {
    spdlog::info("[{}] ReviewService::DeleteReview: review_id={}, is_admin={}", request_id, review_id, is_admin);
    if (!is_admin) {
        throw exception::ForbiddenException("Admin privileges required to remove reviews");
    }
    return review_repo_->Delete(review_id);
}

} // namespace faaliha::faalihamart::service
