#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../model/Review.h"

namespace faaliha::faalihamart::dto {

struct CreateReviewRequestDto {
    int64_t product_id{0};
    int32_t rating{5};
    std::string comment;
};

class ReviewResponseDto {
public:
    int64_t id{0};
    int64_t product_id{0};
    int64_t user_id{0};
    std::string user_name;
    int32_t rating{5};
    std::string comment;
    std::string created_at;

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"id", id},
            {"product_id", product_id},
            {"user_id", user_id},
            {"user_name", user_name},
            {"rating", rating},
            {"comment", comment},
            {"created_at", created_at}
        };
    }

    static ReviewResponseDto FromEntity(const model::Review& review) {
        ReviewResponseDto dto;
        dto.id = review.id_;
        dto.product_id = review.product_id_;
        dto.user_id = review.user_id_;
        dto.user_name = review.user_name_;
        dto.rating = review.rating_;
        dto.comment = review.comment_;
        dto.created_at = review.created_at_;
        return dto;
    }
};

} // namespace faaliha::faalihamart::dto
