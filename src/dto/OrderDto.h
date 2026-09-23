#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../model/Order.h"
#include "../model/OrderItem.h"

namespace faaliha::faalihamart::dto {

struct CheckoutRequestDto {
    std::string payment_method{"MOCK_CREDIT_CARD"};
    std::string shipping_address;
};

struct UpdateOrderStatusDto {
    std::string status; // PENDING, CONFIRMED, SHIPPED, DELIVERED, CANCELLED
};

class OrderItemResponseDto {
public:
    int64_t id{0};
    int64_t product_id{0};
    std::string product_name;
    std::string product_image_url;
    int32_t quantity{0};
    int64_t unit_price_cents{0};
    std::string unit_price_formatted;
    int64_t subtotal_cents{0};
    std::string subtotal_formatted;

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"id", id},
            {"product_id", product_id},
            {"product_name", product_name},
            {"product_image_url", product_image_url},
            {"quantity", quantity},
            {"unit_price_cents", unit_price_cents},
            {"unit_price_formatted", unit_price_formatted},
            {"subtotal_cents", subtotal_cents},
            {"subtotal_formatted", subtotal_formatted}
        };
    }

    static OrderItemResponseDto FromEntity(const model::OrderItem& item) {
        OrderItemResponseDto dto;
        dto.id = item.id_;
        dto.product_id = item.product_id_;
        dto.product_name = item.product_name_;
        dto.product_image_url = item.product_image_url_;
        dto.quantity = item.quantity_;
        dto.unit_price_cents = item.unit_price_cents_.GetCents();
        dto.unit_price_formatted = item.unit_price_cents_.ToString();
        model::Money subtotal = item.GetSubtotal();
        dto.subtotal_cents = subtotal.GetCents();
        dto.subtotal_formatted = subtotal.ToString();
        return dto;
    }
};

class OrderResponseDto {
public:
    int64_t id{0};
    int64_t buyer_id{0};
    std::string buyer_name;
    std::string buyer_email;
    std::string status;
    int64_t total_amount_cents{0};
    std::string total_amount_formatted;
    std::string created_at;
    std::vector<OrderItemResponseDto> items;

    [[nodiscard]] nlohmann::json ToJson() const {
        nlohmann::json itemsArr = nlohmann::json::array();
        for (const auto& item : items) {
            itemsArr.push_back(item.ToJson());
        }
        return {
            {"id", id},
            {"buyer_id", buyer_id},
            {"buyer_name", buyer_name},
            {"buyer_email", buyer_email},
            {"status", status},
            {"total_amount_cents", total_amount_cents},
            {"total_amount_formatted", total_amount_formatted},
            {"created_at", created_at},
            {"items", itemsArr}
        };
    }

    static OrderResponseDto FromEntity(const model::Order& order) {
        OrderResponseDto dto;
        dto.id = order.id_;
        dto.buyer_id = order.buyer_id_;
        dto.buyer_name = order.buyer_name_;
        dto.buyer_email = order.buyer_email_;
        dto.status = model::OrderStatusToString(order.status_);
        dto.total_amount_cents = order.total_amount_cents_.GetCents();
        dto.total_amount_formatted = order.total_amount_cents_.ToString();
        dto.created_at = order.created_at_;
        for (const auto& item : order.items_) {
            dto.items.push_back(OrderItemResponseDto::FromEntity(item));
        }
        return dto;
    }
};

} // namespace faaliha::faalihamart::dto
