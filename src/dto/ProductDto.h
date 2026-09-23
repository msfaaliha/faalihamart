#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "../model/Product.h"

namespace faaliha::faalihamart::dto {

struct ProductRequestDto {
    std::string name;
    std::string description;
    int64_t price_cents{0};
    int32_t stock_qty{0};
    std::string category;
    std::string image_url;
};

struct ProductFilterDto {
    std::optional<std::string> category;
    std::optional<std::string> keyword;
    std::optional<int64_t> min_price_cents;
    std::optional<int64_t> max_price_cents;
    std::optional<int64_t> seller_id;
    int page{1};
    int page_size{20};
};

class ProductResponseDto {
public:
    int64_t id{0};
    int64_t seller_id{0};
    std::string seller_name;
    std::string name;
    std::string description;
    int64_t price_cents{0};
    double price_dollars{0.0};
    double price_rupees{0.0};
    std::string price_formatted;
    int32_t stock_qty{0};
    std::string category;
    std::string image_url;
    std::string created_at;

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"id", id},
            {"seller_id", seller_id},
            {"seller_name", seller_name},
            {"name", name},
            {"description", description},
            {"price_cents", price_cents},
            {"price_dollars", price_dollars},
            {"price_rupees", price_rupees},
            {"price_formatted", price_formatted},
            {"stock_qty", stock_qty},
            {"category", category},
            {"image_url", image_url},
            {"created_at", created_at}
        };
    }

    class Builder {
    public:
        Builder& WithId(int64_t id) { dto_.id = id; return *this; }
        Builder& WithSellerId(int64_t seller_id) { dto_.seller_id = seller_id; return *this; }
        Builder& WithSellerName(std::string name) { dto_.seller_name = std::move(name); return *this; }
        Builder& WithName(std::string name) { dto_.name = std::move(name); return *this; }
        Builder& WithDescription(std::string desc) { dto_.description = std::move(desc); return *this; }
        Builder& WithPrice(const model::Money& price) {
            dto_.price_cents = price.GetCents();
            dto_.price_dollars = price.ToDollars();
            dto_.price_rupees = price.ToRupees();
            dto_.price_formatted = price.ToString();
            return *this;
        }
        Builder& WithStockQty(int32_t stock_qty) { dto_.stock_qty = stock_qty; return *this; }
        Builder& WithCategory(std::string cat) { dto_.category = std::move(cat); return *this; }
        Builder& WithImageUrl(std::string url) { dto_.image_url = std::move(url); return *this; }
        Builder& WithCreatedAt(std::string created_at) { dto_.created_at = std::move(created_at); return *this; }
        ProductResponseDto Build() { return std::move(dto_); }
    private:
        ProductResponseDto dto_;
    };

    static ProductResponseDto FromEntity(const model::Product& product) {
        return Builder()
            .WithId(product.id_)
            .WithSellerId(product.seller_id_)
            .WithSellerName(product.seller_name_)
            .WithName(product.name_)
            .WithDescription(product.description_)
            .WithPrice(product.price_cents_)
            .WithStockQty(product.stock_qty_)
            .WithCategory(product.category_)
            .WithImageUrl(product.image_url_)
            .WithCreatedAt(product.created_at_)
            .Build();
    }
};

} // namespace faaliha::faalihamart::dto
