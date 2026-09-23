#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../model/CartItem.h"
#include "../model/Money.h"

namespace faaliha::faalihamart::dto {

struct AddToCartRequestDto {
    int64_t product_id{0};
    int32_t quantity{1};
};

struct UpdateCartRequestDto {
    int32_t quantity{1};
};

class CartItemResponseDto {
public:
    int64_t id{0};
    int64_t product_id{0};
    std::string product_name;
    std::string product_image_url;
    int64_t price_cents{0};
    std::string price_formatted;
    int32_t quantity{0};
    int64_t subtotal_cents{0};
    std::string subtotal_formatted;
    int32_t stock_qty{0};

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"id", id},
            {"product_id", product_id},
            {"product_name", product_name},
            {"product_image_url", product_image_url},
            {"price_cents", price_cents},
            {"price_formatted", price_formatted},
            {"quantity", quantity},
            {"subtotal_cents", subtotal_cents},
            {"subtotal_formatted", subtotal_formatted},
            {"stock_qty", stock_qty}
        };
    }

    static CartItemResponseDto FromEntity(const model::CartItem& item) {
        CartItemResponseDto dto;
        dto.id = item.id_;
        dto.product_id = item.product_id_;
        dto.product_name = item.product_name_;
        dto.product_image_url = item.product_image_url_;
        dto.price_cents = item.product_price_cents_.GetCents();
        dto.price_formatted = item.product_price_cents_.ToString();
        dto.quantity = item.quantity_;
        model::Money subtotal = item.GetSubtotal();
        dto.subtotal_cents = subtotal.GetCents();
        dto.subtotal_formatted = subtotal.ToString();
        dto.stock_qty = item.product_stock_qty_;
        return dto;
    }
};

class CartResponseDto {
public:
    std::vector<CartItemResponseDto> items;
    int32_t total_items{0};
    int64_t total_amount_cents{0};
    std::string total_amount_formatted;

    [[nodiscard]] nlohmann::json ToJson() const {
        nlohmann::json itemsArr = nlohmann::json::array();
        for (const auto& it : items) {
            itemsArr.push_back(it.ToJson());
        }
        return {
            {"items", itemsArr},
            {"total_items", total_items},
            {"total_amount_cents", total_amount_cents},
            {"total_amount_formatted", total_amount_formatted}
        };
    }
};

} // namespace faaliha::faalihamart::dto
