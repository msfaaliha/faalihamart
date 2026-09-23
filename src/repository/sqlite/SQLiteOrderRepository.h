#pragma once

#include "../IOrderRepository.h"
#include <sqlite3.h>
#include <mutex>

namespace faaliha::faalihamart::repository::sqlite {

/**
 * @brief SQLite implementation of IOrderRepository.
 */
class SQLiteOrderRepository : public IOrderRepository {
public:
    explicit SQLiteOrderRepository(sqlite3* db);

    std::optional<model::Order> FindById(int64_t id) override;
    std::vector<model::Order> FindByBuyerId(int64_t buyer_id) override;
    std::vector<model::Order> FindBySellerId(int64_t seller_id) override;
    std::vector<model::Order> FindAll() override;
    model::Order CreateOrder(const model::Order& order, const std::vector<model::OrderItem>& items) override;
    bool UpdateStatus(int64_t id, model::OrderStatus new_status) override;
    bool HasDeliveredPurchase(int64_t buyer_id, int64_t product_id) override;
    void GetSellerStats(int64_t seller_id, int64_t& out_order_count, int64_t& out_revenue_cents) override;
    void GetPlatformStats(int64_t& out_order_count, int64_t& out_revenue_cents) override;

private:
    std::vector<model::OrderItem> LoadOrderItems(int64_t order_id);

    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
};

} // namespace faaliha::faalihamart::repository::sqlite
