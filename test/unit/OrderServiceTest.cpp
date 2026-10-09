#include <gtest/gtest.h>
#include <unordered_map>
#include "service/OrderService.h"
#include "repository/IOrderRepository.h"
#include "repository/ICartRepository.h"
#include "service/payment/MockPaymentStrategy.h"
#include "exception/ApiException.h"

using namespace faaliha::faalihamart;

class FakeCartRepository : public repository::ICartRepository {
public:
    std::vector<model::CartItem> FindByUserId(int64_t user_id) override {
        std::vector<model::CartItem> res;
        for (const auto& [_, item] : items_) {
            if (item.user_id_ == user_id) res.push_back(item);
        }
        return res;
    }

    model::CartItem AddOrUpdateItem(int64_t user_id, int64_t product_id, int32_t quantity) override {
        model::CartItem item;
        item.id_ = 1;
        item.user_id_ = user_id;
        item.product_id_ = product_id;
        item.quantity_ = quantity;
        item.product_price_cents_ = model::Money::FromCents(5000);
        items_[item.id_] = item;
        return item;
    }

    bool UpdateQuantity(int64_t cart_item_id, int64_t user_id, int32_t quantity) override {
        auto it = items_.find(cart_item_id);
        if (it != items_.end() && it->second.user_id_ == user_id) {
            it->second.quantity_ = quantity;
            return true;
        }
        return false;
    }

    bool RemoveItem(int64_t cart_item_id, int64_t user_id) override {
        auto it = items_.find(cart_item_id);
        if (it != items_.end() && it->second.user_id_ == user_id) {
            items_.erase(it);
            return true;
        }
        return false;
    }

    bool ClearByUserId(int64_t user_id) override {
        for (auto it = items_.begin(); it != items_.end();) {
            if (it->second.user_id_ == user_id) it = items_.erase(it);
            else ++it;
        }
        return true;
    }

    void AddFakeItem(const model::CartItem& item) {
        items_[item.id_] = item;
    }

private:
    std::unordered_map<int64_t, model::CartItem> items_;
};

class FakeOrderRepository : public repository::IOrderRepository {
public:
    std::optional<model::Order> FindById(int64_t id) override {
        auto it = orders_.find(id);
        if (it != orders_.end()) return it->second;
        return std::nullopt;
    }

    std::vector<model::Order> FindByBuyerId(int64_t buyer_id) override {
        std::vector<model::Order> res;
        for (const auto& [_, o] : orders_) {
            if (o.buyer_id_ == buyer_id) res.push_back(o);
        }
        return res;
    }

    std::vector<model::Order> FindBySellerId(int64_t /*seller_id*/) override {
        return {};
    }

    std::vector<model::Order> FindAll() override {
        std::vector<model::Order> res;
        for (const auto& [_, o] : orders_) res.push_back(o);
        return res;
    }

    model::Order CreateOrderWithItems(int64_t buyer_id,
                                      const std::vector<model::CartItem>& items,
                                      const model::Money& total) override {
        model::Order o;
        o.id_ = next_id_++;
        o.buyer_id_ = buyer_id;
        o.status_ = model::OrderStatus::PENDING;
        o.total_amount_cents_ = total;
        for (const auto& ci : items) {
            model::OrderItem oi;
            oi.order_id_ = o.id_;
            oi.product_id_ = ci.product_id_;
            oi.quantity_ = ci.quantity_;
            oi.unit_price_cents_ = ci.product_price_cents_;
            o.items_.push_back(oi);
        }
        orders_[o.id_] = o;
        return o;
    }

    bool UpdateStatus(int64_t order_id, model::OrderStatus new_status) override {
        auto it = orders_.find(order_id);
        if (it == orders_.end()) return false;
        it->second.status_ = new_status;
        return true;
    }

    int64_t Count() override {
        return static_cast<int64_t>(orders_.size());
    }

    model::Money CalculateTotalPlatformRevenue() override {
        model::Money total = model::Money::FromCents(0);
        for (const auto& [_, o] : orders_) total = total + o.total_amount_cents_;
        return total;
    }

    model::Money CalculateSellerRevenue(int64_t /*seller_id*/) override {
        return model::Money::FromCents(0);
    }

    int64_t CountSellerOrders(int64_t /*seller_id*/) override {
        return 0;
    }

    void AddFakeOrder(const model::Order& o) {
        orders_[o.id_] = o;
    }

private:
    int64_t next_id_{1};
    std::unordered_map<int64_t, model::Order> orders_;
};

TEST(OrderServiceTest, CheckoutRejectsEmptyCart) {
    auto order_repo = std::make_shared<FakeOrderRepository>();
    auto cart_repo = std::make_shared<FakeCartRepository>();
    auto payment_strategy = std::make_shared<service::payment::MockPaymentStrategy>();
    service::OrderService order_svc(order_repo, cart_repo, payment_strategy);

    dto::CheckoutRequestDto req;
    req.shipping_address = "123 Main St, Test City";
    req.payment_method = "MOCK_CARD";

    EXPECT_THROW(
        order_svc.Checkout(10, req, "req-order-1"),
        exception::ValidationException
    );
}

TEST(OrderServiceTest, ValidatesOrderStatusTransitions) {
    auto order_repo = std::make_shared<FakeOrderRepository>();
    auto cart_repo = std::make_shared<FakeCartRepository>();
    auto payment_strategy = std::make_shared<service::payment::MockPaymentStrategy>();
    service::OrderService order_svc(order_repo, cart_repo, payment_strategy);

    model::Order fake_order;
    fake_order.id_ = 100;
    fake_order.buyer_id_ = 4;
    fake_order.status_ = model::OrderStatus::PENDING;
    fake_order.total_amount_cents_ = model::Money::FromCents(9999);
    order_repo->AddFakeOrder(fake_order);

    // Transition PENDING -> CONFIRMED is valid
    auto updated = order_svc.UpdateOrderStatus(100, "CONFIRMED", 4, true, "req-order-2");
    EXPECT_EQ(updated.status, "CONFIRMED");

    // Invalid transition: CANCELLED -> DELIVERED should throw validation error
    EXPECT_THROW(
        order_svc.UpdateOrderStatus(100, "INVALID_STATE", 4, true, "req-order-3"),
        exception::ValidationException
    );
}
