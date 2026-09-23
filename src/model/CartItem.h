#pragma once

#include <cstdint>
#include <string>
#include "Money.h"

namespace faaliha::faalihamart::model {

/**
 * @brief Cart Item domain model.
 */
struct CartItem {
    int64_t id_{0};
    int64_t user_id_{0};
    int64_t product_id_{0};
    int32_t quantity_{1};
    std::string created_at_;

    // Joined product details for cart display
    std::string product_name_;
    Money product_price_cents_{Money::FromCents(0)};
    std::string product_image_url_;
    int32_t product_stock_qty_{0};
    int64_t seller_id_{0};

    [[nodiscard]] Money GetSubtotal() const noexcept {
        return product_price_cents_ * quantity_;
    }
};

} // namespace faaliha::faalihamart::model
