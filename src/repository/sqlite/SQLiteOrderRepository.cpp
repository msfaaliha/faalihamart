#include "SQLiteOrderRepository.h"
#include "SQLiteHelper.h"
#include <stdexcept>

namespace faaliha::faalihamart::repository::sqlite {

SQLiteOrderRepository::SQLiteOrderRepository(sqlite3* db) : db_(db) {}

std::vector<model::OrderItem> SQLiteOrderRepository::LoadOrderItems(int64_t order_id) {
    const std::string sql = 
        "SELECT oi.id, oi.order_id, oi.product_id, oi.quantity, oi.unit_price_cents, "
        "       p.name, p.image_url, p.seller_id "
        "FROM order_items oi "
        "JOIN products p ON oi.product_id = p.id "
        "WHERE oi.order_id = ? "
        "ORDER BY oi.id ASC;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, order_id);

    std::vector<model::OrderItem> items;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::OrderItem item;
        item.id_ = sqlite3_column_int64(stmt.get(), 0);
        item.order_id_ = sqlite3_column_int64(stmt.get(), 1);
        item.product_id_ = sqlite3_column_int64(stmt.get(), 2);
        item.quantity_ = sqlite3_column_int(stmt.get(), 3);
        item.unit_price_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 4));
        item.product_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        const char* img = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        item.product_image_url_ = img ? img : "";
        item.seller_id_ = sqlite3_column_int64(stmt.get(), 7);
        items.push_back(item);
    }
    return items;
}

std::optional<model::Order> SQLiteOrderRepository::FindById(int64_t id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, u.name, u.email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "WHERE o.id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Order order;
        order.id_ = sqlite3_column_int64(stmt.get(), 0);
        order.buyer_id_ = sqlite3_column_int64(stmt.get(), 1);
        order.status_ = model::StringToOrderStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2)));
        order.total_amount_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 3));
        order.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        order.buyer_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        order.buyer_email_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        order.items_ = LoadOrderItems(order.id_);
        return order;
    }
    return std::nullopt;
}

std::vector<model::Order> SQLiteOrderRepository::FindByBuyerId(int64_t buyer_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, u.name, u.email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "WHERE o.buyer_id = ? "
        "ORDER BY o.id DESC;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, buyer_id);

    std::vector<model::Order> orders;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Order order;
        order.id_ = sqlite3_column_int64(stmt.get(), 0);
        order.buyer_id_ = sqlite3_column_int64(stmt.get(), 1);
        order.status_ = model::StringToOrderStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2)));
        order.total_amount_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 3));
        order.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        order.buyer_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        order.buyer_email_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        order.items_ = LoadOrderItems(order.id_);
        orders.push_back(order);
    }
    return orders;
}

std::vector<model::Order> SQLiteOrderRepository::FindBySellerId(int64_t seller_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT DISTINCT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, u.name, u.email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "JOIN order_items oi ON o.id = oi.order_id "
        "JOIN products p ON oi.product_id = p.id "
        "WHERE p.seller_id = ? "
        "ORDER BY o.id DESC;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, seller_id);

    std::vector<model::Order> orders;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Order order;
        order.id_ = sqlite3_column_int64(stmt.get(), 0);
        order.buyer_id_ = sqlite3_column_int64(stmt.get(), 1);
        order.status_ = model::StringToOrderStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2)));
        order.total_amount_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 3));
        order.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        order.buyer_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        order.buyer_email_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        order.items_ = LoadOrderItems(order.id_);
        orders.push_back(order);
    }
    return orders;
}

std::vector<model::Order> SQLiteOrderRepository::FindAll() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, u.name, u.email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "ORDER BY o.id DESC;";

    auto stmt = PrepareStatement(db_, sql);

    std::vector<model::Order> orders;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Order order;
        order.id_ = sqlite3_column_int64(stmt.get(), 0);
        order.buyer_id_ = sqlite3_column_int64(stmt.get(), 1);
        order.status_ = model::StringToOrderStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2)));
        order.total_amount_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 3));
        order.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        order.buyer_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        order.buyer_email_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        order.items_ = LoadOrderItems(order.id_);
        orders.push_back(order);
    }
    return orders;
}

model::Order SQLiteOrderRepository::CreateOrder(const model::Order& order, const std::vector<model::OrderItem>& items) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    // Transaction begin
    char* errmsg = nullptr;
    if (sqlite3_exec(db_, "BEGIN TRANSACTION;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::string err = errmsg ? errmsg : "Transaction begin failed";
        sqlite3_free(errmsg);
        throw std::runtime_error(err);
    }

    try {
        const std::string order_sql = 
            "INSERT INTO orders (buyer_id, status, total_amount_cents) VALUES (?, ?, ?);";
        auto order_stmt = PrepareStatement(db_, order_sql);
        sqlite3_bind_int64(order_stmt.get(), 1, order.buyer_id_);
        std::string status_str = model::OrderStatusToString(order.status_);
        sqlite3_bind_text(order_stmt.get(), 2, status_str.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(order_stmt.get(), 3, order.total_amount_cents_.GetCents());

        if (sqlite3_step(order_stmt.get()) != SQLITE_DONE) {
            throw std::runtime_error("Failed to insert order: " + std::string(sqlite3_errmsg(db_)));
        }

        int64_t new_order_id = sqlite3_last_insert_rowid(db_);

        const std::string item_sql = 
            "INSERT INTO order_items (order_id, product_id, quantity, unit_price_cents) VALUES (?, ?, ?, ?);";
        const std::string stock_sql = 
            "UPDATE products SET stock_qty = stock_qty - ? WHERE id = ? AND stock_qty >= ?;";

        for (const auto& it : items) {
            // Check & decrement inventory
            auto stock_stmt = PrepareStatement(db_, stock_sql);
            sqlite3_bind_int(stock_stmt.get(), 1, it.quantity_);
            sqlite3_bind_int64(stock_stmt.get(), 2, it.product_id_);
            sqlite3_bind_int(stock_stmt.get(), 3, it.quantity_);
            if (sqlite3_step(stock_stmt.get()) != SQLITE_DONE || sqlite3_changes(db_) == 0) {
                throw std::runtime_error("Insufficient stock for product ID " + std::to_string(it.product_id_));
            }

            // Insert line item
            auto item_stmt = PrepareStatement(db_, item_sql);
            sqlite3_bind_int64(item_stmt.get(), 1, new_order_id);
            sqlite3_bind_int64(item_stmt.get(), 2, it.product_id_);
            sqlite3_bind_int(item_stmt.get(), 3, it.quantity_);
            sqlite3_bind_int64(item_stmt.get(), 4, it.unit_price_cents_.GetCents());
            if (sqlite3_step(item_stmt.get()) != SQLITE_DONE) {
                throw std::runtime_error("Failed to insert order item: " + std::string(sqlite3_errmsg(db_)));
            }
        }

        if (sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
            std::string err = errmsg ? errmsg : "Transaction commit failed";
            sqlite3_free(errmsg);
            throw std::runtime_error(err);
        }

        model::Order created = order;
        created.id_ = new_order_id;
        created.items_ = items;
        return created;
    } catch (...) {
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        throw;
    }
}

bool SQLiteOrderRepository::UpdateStatus(int64_t id, model::OrderStatus new_status) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "UPDATE orders SET status = ? WHERE id = ?;";
    auto stmt = PrepareStatement(db_, sql);
    std::string s = model::OrderStatusToString(new_status);
    sqlite3_bind_text(stmt.get(), 1, s.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt.get(), 2, id);

    return sqlite3_step(stmt.get()) == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

bool SQLiteOrderRepository::HasDeliveredPurchase(int64_t buyer_id, int64_t product_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT 1 FROM orders o "
        "JOIN order_items oi ON o.id = oi.order_id "
        "WHERE o.buyer_id = ? AND oi.product_id = ? AND o.status = 'DELIVERED' "
        "LIMIT 1;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, buyer_id);
    sqlite3_bind_int64(stmt.get(), 2, product_id);

    return sqlite3_step(stmt.get()) == SQLITE_ROW;
}

void SQLiteOrderRepository::GetSellerStats(int64_t seller_id, int64_t& out_order_count, int64_t& out_revenue_cents) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    out_order_count = 0;
    out_revenue_cents = 0;

    const std::string sql = 
        "SELECT COUNT(DISTINCT o.id), COALESCE(SUM(oi.quantity * oi.unit_price_cents), 0) "
        "FROM orders o "
        "JOIN order_items oi ON o.id = oi.order_id "
        "JOIN products p ON oi.product_id = p.id "
        "WHERE p.seller_id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, seller_id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        out_order_count = sqlite3_column_int64(stmt.get(), 0);
        out_revenue_cents = sqlite3_column_int64(stmt.get(), 1);
    }
}

void SQLiteOrderRepository::GetPlatformStats(int64_t& out_order_count, int64_t& out_revenue_cents) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    out_order_count = 0;
    out_revenue_cents = 0;

    const std::string sql = 
        "SELECT COUNT(id), COALESCE(SUM(total_amount_cents), 0) FROM orders;";

    auto stmt = PrepareStatement(db_, sql);
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        out_order_count = sqlite3_column_int64(stmt.get(), 0);
        out_revenue_cents = sqlite3_column_int64(stmt.get(), 1);
    }
}

} // namespace faaliha::faalihamart::repository::sqlite
