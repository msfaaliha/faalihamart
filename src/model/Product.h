#pragma once

#include <cstdint>
#include <string>
#include "Money.h"

namespace faaliha::faalihamart::model {

/**
 * @brief Product domain model.
 */
struct Product {
    int64_t id_{0};
    int64_t seller_id_{0};
    std::string name_;
    std::string description_;
    Money price_cents_{Money::FromCents(0)};
    int32_t stock_qty_{0};
    std::string category_;
    std::string image_url_;
    std::string created_at_;

    // Optional joined seller info
    std::string seller_name_;
};

} // namespace faaliha::faalihamart::model
