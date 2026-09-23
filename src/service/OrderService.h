#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../repository/IOrderRepository.h"
#include "../repository/ICartRepository.h"
#include "payment/IPaymentStrategy.h"
#include "../dto/OrderDto.h"

namespace faaliha::faalihamart::service {

/**
 * @brief Handles order placement, mock payment processing, status workflow, and order history.
 */
class OrderService {
public:
    OrderService(std::shared_ptr<repository::IOrderRepository> order_repo,
                 std::shared_ptr<repository::ICartRepository> cart_repo,
                 std::shared_ptr<payment::IPaymentStrategy> payment_strategy);

    /**
     * @brief Place an order from active cart contents via payment strategy.
     * @param buyer_id Authenticated buyer ID.
     * @param req Checkout request details.
     * @param request_id Request tracking ID.
     * @return OrderResponseDto of placed order.
     */
    dto::OrderResponseDto Checkout(int64_t buyer_id, const dto::CheckoutRequestDto& req, const std::string& request_id = "");

    /**
     * @brief Retrieve buyer's order history.
     * @param buyer_id Buyer user ID.
     * @param request_id Request tracking ID.
     * @return List of OrderResponseDto.
     */
    std::vector<dto::OrderResponseDto> GetBuyerOrders(int64_t buyer_id, const std::string& request_id = "");

    /**
     * @brief Retrieve incoming orders containing products for a seller.
     * @param seller_id Seller user ID.
     * @param request_id Request tracking ID.
     * @return List of OrderResponseDto.
     */
    std::vector<dto::OrderResponseDto> GetSellerOrders(int64_t seller_id, const std::string& request_id = "");

    /**
     * @brief Get details of a single order.
     * @param order_id Order ID.
     * @param requesting_user_id Authenticated user ID.
     * @param is_admin Whether user is admin.
     * @param request_id Request tracking ID.
     * @return OrderResponseDto.
     */
    dto::OrderResponseDto GetOrderById(int64_t order_id, int64_t requesting_user_id, bool is_admin, const std::string& request_id = "");

    /**
     * @brief Update order status (O2 workflow: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED).
     * @param order_id Order ID.
     * @param new_status Target status string.
     * @param requesting_user_id Authenticated user ID.
     * @param is_admin Whether requesting user is Admin.
     * @param request_id Request tracking ID.
     * @return Updated OrderResponseDto.
     */
    dto::OrderResponseDto UpdateOrderStatus(int64_t order_id, const std::string& new_status, int64_t requesting_user_id, bool is_admin, const std::string& request_id = "");

private:
    std::shared_ptr<repository::IOrderRepository> order_repo_;
    std::shared_ptr<repository::ICartRepository> cart_repo_;
    std::shared_ptr<payment::IPaymentStrategy> payment_strategy_;
};

} // namespace faaliha::faalihamart::service
