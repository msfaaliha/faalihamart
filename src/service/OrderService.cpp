#include "OrderService.h"
#include "../util/ValidationUtil.h"
#include "../exception/ApiException.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::service {

OrderService::OrderService(std::shared_ptr<repository::IOrderRepository> order_repo,
                           std::shared_ptr<repository::ICartRepository> cart_repo,
                           std::shared_ptr<payment::IPaymentStrategy> payment_strategy)
    : order_repo_(std::move(order_repo)),
      cart_repo_(std::move(cart_repo)),
      payment_strategy_(std::move(payment_strategy)) {}

dto::OrderResponseDto OrderService::Checkout(int64_t buyer_id, const dto::CheckoutRequestDto& req, const std::string& request_id) {
    spdlog::info("[{}] OrderService::Checkout: buyer_id={}", request_id, buyer_id);

    // 1. Validate inputs
    util::ValidationUtil::ValidateNonEmpty("shipping_address", req.shipping_address);

    // 2. Fetch cart items
    auto cart_items = cart_repo_->FindByUserId(buyer_id);
    if (cart_items.empty()) {
        throw exception::ValidationException("Cannot checkout with an empty cart");
    }

    // 3. Compute total amount and prepare order items
    model::Money total_amount{model::Money::FromCents(0)};
    std::vector<model::OrderItem> order_items;
    order_items.reserve(cart_items.size());

    for (const auto& ci : cart_items) {
        if (ci.quantity_ > ci.product_stock_qty_) {
            throw exception::ConflictException(
                "Product '" + ci.product_name_ + "' has insufficient stock (" + 
                std::to_string(ci.product_stock_qty_) + " available)"
            );
        }

        model::OrderItem oi;
        oi.product_id_ = ci.product_id_;
        oi.quantity_ = ci.quantity_;
        oi.unit_price_cents_ = ci.product_price_cents_;
        oi.product_name_ = ci.product_name_;
        oi.product_image_url_ = ci.product_image_url_;
        oi.seller_id_ = ci.seller_id_;

        total_amount = total_amount + oi.GetSubtotal();
        order_items.push_back(oi);
    }

    // 4. Process payment via strategy
    auto payment_res = payment_strategy_->ProcessPayment(total_amount, req.payment_method);
    if (!payment_res.success) {
        spdlog::error("[{}] Payment failed: {}", request_id, payment_res.message);
        throw exception::ValidationException("Payment authorization failed: " + payment_res.message);
    }

    // 5. Create order entity and insert via transaction in repository
    model::Order order;
    order.buyer_id_ = buyer_id;
    order.status_ = model::OrderStatus::CONFIRMED; // Mock payment successful -> confirmed
    order.total_amount_cents_ = total_amount;

    model::Order created = order_repo_->CreateOrder(order, order_items);

    // 6. Clear user's cart on success
    cart_repo_->ClearCart(buyer_id);

    spdlog::info("[{}] Order created successfully: order_id={}, amount={}",
                 request_id, created.id_, created.total_amount_cents_.ToString());

    auto fetched = order_repo_->FindById(created.id_);
    return dto::OrderResponseDto::FromEntity(fetched ? *fetched : created);
}

std::vector<dto::OrderResponseDto> OrderService::GetBuyerOrders(int64_t buyer_id, const std::string& request_id) {
    spdlog::info("[{}] OrderService::GetBuyerOrders: buyer_id={}", request_id, buyer_id);
    auto orders = order_repo_->FindByBuyerId(buyer_id);
    std::vector<dto::OrderResponseDto> dtos;
    dtos.reserve(orders.size());
    for (const auto& ord : orders) {
        dtos.push_back(dto::OrderResponseDto::FromEntity(ord));
    }
    return dtos;
}

std::vector<dto::OrderResponseDto> OrderService::GetSellerOrders(int64_t seller_id, const std::string& request_id) {
    spdlog::info("[{}] OrderService::GetSellerOrders: seller_id={}", request_id, seller_id);
    auto orders = order_repo_->FindBySellerId(seller_id);
    std::vector<dto::OrderResponseDto> dtos;
    dtos.reserve(orders.size());
    for (const auto& ord : orders) {
        dtos.push_back(dto::OrderResponseDto::FromEntity(ord));
    }
    return dtos;
}

dto::OrderResponseDto OrderService::GetOrderById(int64_t order_id, int64_t requesting_user_id, bool is_admin, const std::string& request_id) {
    spdlog::info("[{}] OrderService::GetOrderById: order_id={}, user_id={}, is_admin={}",
                 request_id, order_id, requesting_user_id, is_admin);

    auto ord_opt = order_repo_->FindById(order_id);
    if (!ord_opt.has_value()) {
        throw exception::NotFoundException("Order not found");
    }

    if (!is_admin && ord_opt->buyer_id_ != requesting_user_id) {
        // Check if user is a seller of any item in this order
        bool is_order_seller = false;
        for (const auto& it : ord_opt->items_) {
            if (it.seller_id_ == requesting_user_id) {
                is_order_seller = true;
                break;
            }
        }
        if (!is_order_seller) {
            throw exception::ForbiddenException("You do not have access to this order");
        }
    }

    return dto::OrderResponseDto::FromEntity(*ord_opt);
}

dto::OrderResponseDto OrderService::UpdateOrderStatus(int64_t order_id, const std::string& new_status_str, int64_t requesting_user_id, bool is_admin, const std::string& request_id) {
    spdlog::info("[{}] OrderService::UpdateOrderStatus: order_id={}, status={}, user_id={}",
                 request_id, order_id, new_status_str, requesting_user_id);

    auto ord_opt = order_repo_->FindById(order_id);
    if (!ord_opt.has_value()) {
        throw exception::NotFoundException("Order not found");
    }

    if (!is_admin) {
        bool is_seller = false;
        for (const auto& item : ord_opt->items_) {
            if (item.seller_id_ == requesting_user_id) {
                is_seller = true;
                break;
            }
        }
        if (!is_seller) {
            throw exception::ForbiddenException("Only sellers or admins can update order status");
        }
    }

    model::OrderStatus new_status = model::StringToOrderStatus(new_status_str);
    order_repo_->UpdateStatus(order_id, new_status);

    auto updated = order_repo_->FindById(order_id);
    return dto::OrderResponseDto::FromEntity(updated ? *updated : *ord_opt);
}

} // namespace faaliha::faalihamart::service
