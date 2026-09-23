#include "CartService.h"
#include "../util/ValidationUtil.h"
#include "../exception/ApiException.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::service {

CartService::CartService(std::shared_ptr<repository::ICartRepository> cart_repo,
                         std::shared_ptr<repository::IProductRepository> product_repo)
    : cart_repo_(std::move(cart_repo)), product_repo_(std::move(product_repo)) {}

dto::CartResponseDto CartService::GetUserCart(int64_t user_id, const std::string& request_id) {
    spdlog::info("[{}] CartService::GetUserCart: user_id={}", request_id, user_id);

    auto items = cart_repo_->FindByUserId(user_id);
    dto::CartResponseDto response;
    model::Money total{model::Money::FromCents(0)};
    int32_t count = 0;

    for (const auto& item : items) {
        dto::CartItemResponseDto itemDto = dto::CartItemResponseDto::FromEntity(item);
        total = total + item.GetSubtotal();
        count += item.quantity_;
        response.items.push_back(itemDto);
    }

    response.total_items = count;
    response.total_amount_cents = total.GetCents();
    response.total_amount_formatted = total.ToString();
    return response;
}

dto::CartResponseDto CartService::AddItem(int64_t user_id, const dto::AddToCartRequestDto& req, const std::string& request_id) {
    spdlog::info("[{}] CartService::AddItem: user_id={}, product_id={}, qty={}",
                 request_id, user_id, req.product_id, req.quantity);

    util::ValidationUtil::ValidatePositive("product_id", req.product_id);
    util::ValidationUtil::ValidatePositive("quantity", req.quantity);

    auto prod_opt = product_repo_->FindById(req.product_id);
    if (!prod_opt.has_value()) {
        throw exception::NotFoundException("Product not found");
    }

    // Check if item already in cart
    auto existing_item = cart_repo_->FindByUserAndProduct(user_id, req.product_id);
    int32_t new_qty = req.quantity;
    if (existing_item.has_value()) {
        new_qty += existing_item->quantity_;
    }

    if (new_qty > prod_opt->stock_qty_) {
        throw exception::ValidationException(
            "Cannot add more items than available in stock",
            {"stock_qty: available " + std::to_string(prod_opt->stock_qty_)}
        );
    }

    model::CartItem item;
    if (existing_item.has_value()) {
        item.id_ = existing_item->id_;
    }
    item.user_id_ = user_id;
    item.product_id_ = req.product_id;
    item.quantity_ = new_qty;

    cart_repo_->Save(item);
    return GetUserCart(user_id, request_id);
}

dto::CartResponseDto CartService::UpdateItemQuantity(int64_t user_id, int64_t cart_item_id, int32_t quantity, const std::string& request_id) {
    spdlog::info("[{}] CartService::UpdateItemQuantity: user_id={}, item_id={}, new_qty={}",
                 request_id, user_id, cart_item_id, quantity);

    if (quantity <= 0) {
        return RemoveItem(user_id, cart_item_id, request_id);
    }

    auto existing_item = cart_repo_->FindById(cart_item_id);
    if (!existing_item.has_value() || existing_item->user_id_ != user_id) {
        throw exception::NotFoundException("Cart item not found");
    }

    auto prod_opt = product_repo_->FindById(existing_item->product_id_);
    if (!prod_opt.has_value()) {
        throw exception::NotFoundException("Associated product no longer exists");
    }

    if (quantity > prod_opt->stock_qty_) {
        throw exception::ValidationException(
            "Requested quantity exceeds available stock",
            {"stock_qty: available " + std::to_string(prod_opt->stock_qty_)}
        );
    }

    model::CartItem updated = *existing_item;
    updated.quantity_ = quantity;
    cart_repo_->Save(updated);

    return GetUserCart(user_id, request_id);
}

dto::CartResponseDto CartService::RemoveItem(int64_t user_id, int64_t cart_item_id, const std::string& request_id) {
    spdlog::info("[{}] CartService::RemoveItem: user_id={}, item_id={}", request_id, user_id, cart_item_id);

    cart_repo_->Delete(cart_item_id, user_id);
    return GetUserCart(user_id, request_id);
}

void CartService::ClearUserCart(int64_t user_id, const std::string& request_id) {
    spdlog::info("[{}] CartService::ClearUserCart: user_id={}", request_id, user_id);
    cart_repo_->ClearCart(user_id);
}

} // namespace faaliha::faalihamart::service
