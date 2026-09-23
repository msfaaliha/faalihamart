#include "SQLiteCartRepository.h"
#include "SQLiteHelper.h"
#include <stdexcept>

namespace faaliha::faalihamart::repository::sqlite {

SQLiteCartRepository::SQLiteCartRepository(sqlite3* db) : db_(db) {}

std::vector<model::CartItem> SQLiteCartRepository::FindByUserId(int64_t user_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT c.id, c.user_id, c.product_id, c.quantity, c.created_at, "
        "       p.name, p.price_cents, p.image_url, p.stock_qty, p.seller_id "
        "FROM cart_items c "
        "JOIN products p ON c.product_id = p.id "
        "WHERE c.user_id = ? "
        "ORDER BY c.id ASC;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, user_id);

    std::vector<model::CartItem> items;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::CartItem item;
        item.id_ = sqlite3_column_int64(stmt.get(), 0);
        item.user_id_ = sqlite3_column_int64(stmt.get(), 1);
        item.product_id_ = sqlite3_column_int64(stmt.get(), 2);
        item.quantity_ = sqlite3_column_int(stmt.get(), 3);
        item.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        item.product_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        item.product_price_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 6));
        const char* img = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 7));
        item.product_image_url_ = img ? img : "";
        item.product_stock_qty_ = sqlite3_column_int(stmt.get(), 8);
        item.seller_id_ = sqlite3_column_int64(stmt.get(), 9);
        items.push_back(item);
    }
    return items;
}

std::optional<model::CartItem> SQLiteCartRepository::FindById(int64_t id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT c.id, c.user_id, c.product_id, c.quantity, c.created_at, "
        "       p.name, p.price_cents, p.image_url, p.stock_qty, p.seller_id "
        "FROM cart_items c "
        "JOIN products p ON c.product_id = p.id "
        "WHERE c.id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::CartItem item;
        item.id_ = sqlite3_column_int64(stmt.get(), 0);
        item.user_id_ = sqlite3_column_int64(stmt.get(), 1);
        item.product_id_ = sqlite3_column_int64(stmt.get(), 2);
        item.quantity_ = sqlite3_column_int(stmt.get(), 3);
        item.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        item.product_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        item.product_price_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 6));
        const char* img = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 7));
        item.product_image_url_ = img ? img : "";
        item.product_stock_qty_ = sqlite3_column_int(stmt.get(), 8);
        item.seller_id_ = sqlite3_column_int64(stmt.get(), 9);
        return item;
    }
    return std::nullopt;
}

std::optional<model::CartItem> SQLiteCartRepository::FindByUserAndProduct(int64_t user_id, int64_t product_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT c.id, c.user_id, c.product_id, c.quantity, c.created_at, "
        "       p.name, p.price_cents, p.image_url, p.stock_qty, p.seller_id "
        "FROM cart_items c "
        "JOIN products p ON c.product_id = p.id "
        "WHERE c.user_id = ? AND c.product_id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, user_id);
    sqlite3_bind_int64(stmt.get(), 2, product_id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::CartItem item;
        item.id_ = sqlite3_column_int64(stmt.get(), 0);
        item.user_id_ = sqlite3_column_int64(stmt.get(), 1);
        item.product_id_ = sqlite3_column_int64(stmt.get(), 2);
        item.quantity_ = sqlite3_column_int(stmt.get(), 3);
        item.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        item.product_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        item.product_price_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 6));
        const char* img = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 7));
        item.product_image_url_ = img ? img : "";
        item.product_stock_qty_ = sqlite3_column_int(stmt.get(), 8);
        item.seller_id_ = sqlite3_column_int64(stmt.get(), 9);
        return item;
    }
    return std::nullopt;
}

model::CartItem SQLiteCartRepository::Save(const model::CartItem& item) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "INSERT INTO cart_items (user_id, product_id, quantity) VALUES (?, ?, ?) "
        "ON CONFLICT(user_id, product_id) DO UPDATE SET quantity = excluded.quantity;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, item.user_id_);
    sqlite3_bind_int64(stmt.get(), 2, item.product_id_);
    sqlite3_bind_int(stmt.get(), 3, item.quantity_);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        throw std::runtime_error("Failed to save cart item: " + std::string(sqlite3_errmsg(db_)));
    }

    int64_t row_id = sqlite3_last_insert_rowid(db_);
    model::CartItem saved = item;
    if (saved.id_ == 0) {
        saved.id_ = row_id;
    }
    return saved;
}

bool SQLiteCartRepository::Delete(int64_t id, int64_t user_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "DELETE FROM cart_items WHERE id = ? AND user_id = ?;";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, id);
    sqlite3_bind_int64(stmt.get(), 2, user_id);

    return sqlite3_step(stmt.get()) == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

bool SQLiteCartRepository::ClearCart(int64_t user_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "DELETE FROM cart_items WHERE user_id = ?;";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, user_id);

    return sqlite3_step(stmt.get()) == SQLITE_DONE;
}

} // namespace faaliha::faalihamart::repository::sqlite
