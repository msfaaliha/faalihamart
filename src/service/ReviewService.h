#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../repository/IReviewRepository.h"
#include "../repository/IOrderRepository.h"
#include "../dto/ReviewDto.h"

namespace faaliha::faalihamart::service {

/**
 * @brief Manages product reviews, ratings, and purchase verification.
 */
class ReviewService {
public:
    ReviewService(std::shared_ptr<repository::IReviewRepository> review_repo,
                  std::shared_ptr<repository::IOrderRepository> order_repo);

    /**
     * @brief Get all reviews for a product.
     * @param product_id Product ID.
     * @param request_id Request tracking ID.
     * @return List of ReviewResponseDto.
     */
    std::vector<dto::ReviewResponseDto> GetProductReviews(int64_t product_id, const std::string& request_id = "");

    /**
     * @brief Post a review for a completed/delivered purchase.
     * @param user_id Authenticated buyer ID.
     * @param req Review creation details.
     * @param request_id Request tracking ID.
     * @return Created ReviewResponseDto.
     * @throws ForbiddenException if product was not purchased or not yet delivered.
     */
    dto::ReviewResponseDto AddReview(int64_t user_id, const dto::CreateReviewRequestDto& req, const std::string& request_id = "");

    /**
     * @brief Delete a review (Admin moderation).
     * @param review_id Review ID.
     * @param is_admin Must be true.
     * @param request_id Request tracking ID.
     * @return true if deleted.
     */
    bool DeleteReview(int64_t review_id, bool is_admin, const std::string& request_id = "");

private:
    std::shared_ptr<repository::IReviewRepository> review_repo_;
    std::shared_ptr<repository::IOrderRepository> order_repo_;
};

} // namespace faaliha::faalihamart::service
