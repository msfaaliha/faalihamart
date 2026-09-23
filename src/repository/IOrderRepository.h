#pragma once

#include <vector>
#include <optional>
#include <memory>
#include "../model/Order.h"
#include "../model/OrderItem.h"

namespace faaliha::faalihamart::repository {

/**
 * @brief Repository interface for Orders and Order Items data access.
 */
class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;

    /**
     * @brief Find an order by primary key with all associated line items.
     * @param id Order ID.
     * @return Optional containing the Order with its items.
     */
    virtual std::optional<model::Order> FindById(int64_t id) = 0;

    /**
     * @brief Find all orders placed by a specific buyer.
     * @param buyer_id The buyer's ID.
     * @return Vector of orders.
     */
    virtual std::vector<model::Order> FindByBuyerId(int64_t buyer_id) = 0;

    /**
     * @brief Find all orders containing products belonging to a specific seller.
     * @param seller_id The seller's user ID.
     * @return Vector of orders relevant to the seller.
     */
    virtual std::vector<model::Order> FindBySellerId(int64_t seller_id) = 0;

    /**
     * @brief Find all orders in the entire system (for Admin).
     * @return Vector of all orders.
     */
    virtual std::vector<model::Order> FindAll() = 0;

    /**
     * @brief Create a new order with its line items inside a transaction.
     * @param order Order entity.
     * @param items Vector of order line items.
     * @return Created Order entity with populated ID.
     */
    virtual model::Order CreateOrder(const model::Order& order, const std::vector<model::OrderItem>& items) = 0;

    /**
     * @brief Update status of an order (e.g. PENDING -> CONFIRMED -> SHIPPED -> DELIVERED).
     * @param id Order ID.
     * @param new_status New status.
     * @return true if updated.
     */
    virtual bool UpdateStatus(int64_t id, model::OrderStatus new_status) = 0;

    /**
     * @brief Verify if a buyer has purchased a product and the order is DELIVERED (prerequisite for review).
     * @param buyer_id User ID.
     * @param product_id Product ID.
     * @return true if eligible to review.
     */
    virtual bool HasDeliveredPurchase(int64_t buyer_id, int64_t product_id) = 0;

    /**
     * @brief Aggregate statistics for seller dashboard.
     * @param seller_id Seller user ID.
     * @param out_order_count Output parameter for order count.
     * @param out_revenue_cents Output parameter for total revenue in cents.
     */
    virtual void GetSellerStats(int64_t seller_id, int64_t& out_order_count, int64_t& out_revenue_cents) = 0;

    /**
     * @brief Aggregate platform-wide statistics for Admin.
     * @param out_order_count Output total orders.
     * @param out_revenue_cents Output total revenue in cents.
     */
    virtual void GetPlatformStats(int64_t& out_order_count, int64_t& out_revenue_cents) = 0;
};

} // namespace faaliha::faalihamart::repository
