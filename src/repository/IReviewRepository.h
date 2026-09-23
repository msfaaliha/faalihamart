#pragma once

#include <vector>
#include <optional>
#include <memory>
#include "../model/Review.h"

namespace faaliha::faalihamart::repository {

/**
 * @brief Repository interface for product reviews and ratings.
 */
class IReviewRepository {
public:
    virtual ~IReviewRepository() = default;

    /**
     * @brief Find all reviews for a specific product.
     * @param product_id Product ID.
     * @return Vector of reviews with reviewer names.
     */
    virtual std::vector<model::Review> FindByProductId(int64_t product_id) = 0;

    /**
     * @brief Find a review by user and product.
     * @param user_id Buyer ID.
     * @param product_id Product ID.
     * @return Optional review.
     */
    virtual std::optional<model::Review> FindByUserAndProduct(int64_t user_id, int64_t product_id) = 0;

    /**
     * @brief Submit a new review.
     * @param review Review entity to insert.
     * @return Created review with ID.
     */
    virtual model::Review Create(const model::Review& review) = 0;

    /**
     * @brief Delete a review (Admin moderation).
     * @param id Review ID.
     * @return true if deleted.
     */
    virtual bool Delete(int64_t id) = 0;

    /**
     * @brief Calculate average rating and total review count for a product.
     * @param product_id Product ID.
     * @param out_avg_rating Output average rating (0.0 - 5.0).
     * @param out_count Output total count.
     */
    virtual void GetRatingSummary(int64_t product_id, double& out_avg_rating, int64_t& out_count) = 0;
};

} // namespace faaliha::faalihamart::repository
