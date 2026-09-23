#pragma once

#include <cstdint>
#include <string>
#include "Money.h"

namespace faaliha::faalihamart::model {

/**
 * @brief Order Item domain model.
 */
struct OrderItem {
    int64_t id_{0};
    int64_t order_id_{0};
    int64_t product_id_{0};
    int32_t quantity_{0};
    Money unit_price_cents_{Money::FromCents(0)};

    // Joined fields for display
    std::string product_name_;
    std::string product_image_url_;
    int64_t seller_id_{0};

    [[nodiscard]] Money GetSubtotal() const noexcept {
        return unit_price_cents_ * quantity_;
    }
};

} // namespace faaliha::faalihamart::model
