#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Money.h"
#include "OrderItem.h"

namespace faaliha::faalihamart::model {

enum class OrderStatus {
    PENDING,
    CONFIRMED,
    SHIPPED,
    DELIVERED,
    CANCELLED
};

inline std::string OrderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING: return "PENDING";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::SHIPPED: return "SHIPPED";
        case OrderStatus::DELIVERED: return "DELIVERED";
        case OrderStatus::CANCELLED: return "CANCELLED";
    }
    return "PENDING";
}

inline OrderStatus StringToOrderStatus(const std::string& str) {
    if (str == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (str == "SHIPPED") return OrderStatus::SHIPPED;
    if (str == "DELIVERED") return OrderStatus::DELIVERED;
    if (str == "CANCELLED") return OrderStatus::CANCELLED;
    return OrderStatus::PENDING;
}

/**
 * @brief Order domain model.
 */
struct Order {
    int64_t id_{0};
    int64_t buyer_id_{0};
    OrderStatus status_{OrderStatus::PENDING};
    Money total_amount_cents_{Money::FromCents(0)};
    std::string created_at_;

    // Joined buyer info and nested order items
    std::string buyer_name_;
    std::string buyer_email_;
    std::vector<OrderItem> items_;
};

} // namespace faaliha::faalihamart::model
