#include "SQLiteProductRepository.h"
#include "SQLiteHelper.h"
#include <stdexcept>
#include <sstream>

namespace faaliha::faalihamart::repository::sqlite {

SQLiteProductRepository::SQLiteProductRepository(sqlite3* db) : db_(db) {}

std::optional<model::Product> SQLiteProductRepository::FindById(int64_t id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT p.id, p.seller_id, p.name, p.description, p.price_cents, p.stock_qty, "
        "       p.category, p.image_url, p.created_at, u.name as seller_name "
        "FROM products p "
        "JOIN users u ON p.seller_id = u.id "
        "WHERE p.id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Product prod;
        prod.id_ = sqlite3_column_int64(stmt.get(), 0);
        prod.seller_id_ = sqlite3_column_int64(stmt.get(), 1);
        prod.name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 3));
        prod.description_ = desc ? desc : "";
        prod.price_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 4));
        prod.stock_qty_ = sqlite3_column_int(stmt.get(), 5);
        prod.category_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        const char* img = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 7));
        prod.image_url_ = img ? img : "";
        prod.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 8));
        prod.seller_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 9));
        return prod;
    }
    return std::nullopt;
}

std::vector<model::Product> SQLiteProductRepository::FindAll(const dto::ProductFilterDto& filter) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    std::string sql = 
        "SELECT p.id, p.seller_id, p.name, p.description, p.price_cents, p.stock_qty, "
        "       p.category, p.image_url, p.created_at, u.name as seller_name "
        "FROM products p "
        "JOIN users u ON p.seller_id = u.id "
        "WHERE 1=1 ";

    if (filter.category.has_value() && !filter.category->empty()) {
        sql += " AND p.category = ? ";
    }
    if (filter.keyword.has_value() && !filter.keyword->empty()) {
        sql += " AND (p.name LIKE ? OR p.description LIKE ?) ";
    }
    if (filter.seller_id.has_value()) {
        sql += " AND p.seller_id = ? ";
    }

    sql += " ORDER BY p.id DESC;";

    auto stmt = PrepareStatement(db_, sql);
    int bind_idx = 1;

    if (filter.category.has_value() && !filter.category->empty()) {
        sqlite3_bind_text(stmt.get(), bind_idx++, filter.category->c_str(), -1, SQLITE_TRANSIENT);
    }
    std::string kw_pattern;
    if (filter.keyword.has_value() && !filter.keyword->empty()) {
        kw_pattern = "%" + filter.keyword.value() + "%";
        sqlite3_bind_text(stmt.get(), bind_idx++, kw_pattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt.get(), bind_idx++, kw_pattern.c_str(), -1, SQLITE_TRANSIENT);
    }
    if (filter.seller_id.has_value()) {
        sqlite3_bind_int64(stmt.get(), bind_idx++, filter.seller_id.value());
    }

    std::vector<model::Product> products;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Product prod;
        prod.id_ = sqlite3_column_int64(stmt.get(), 0);
        prod.seller_id_ = sqlite3_column_int64(stmt.get(), 1);
        prod.name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 3));
        prod.description_ = desc ? desc : "";
        prod.price_cents_ = model::Money::FromCents(sqlite3_column_int64(stmt.get(), 4));
        prod.stock_qty_ = sqlite3_column_int(stmt.get(), 5);
        prod.category_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        const char* img = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 7));
        prod.image_url_ = img ? img : "";
        prod.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 8));
        prod.seller_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 9));
        products.push_back(prod);
    }
    return products;
}

std::vector<model::Product> SQLiteProductRepository::FindBySellerId(int64_t seller_id) {
    dto::ProductFilterDto filter;
    filter.seller_id = seller_id;
    return FindAll(filter);
}

model::Product SQLiteProductRepository::Create(const model::Product& product) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "INSERT INTO products (seller_id, name, description, price_cents, stock_qty, category, image_url) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, product.seller_id_);
    sqlite3_bind_text(stmt.get(), 2, product.name_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 3, product.description_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt.get(), 4, product.price_cents_.GetCents());
    sqlite3_bind_int(stmt.get(), 5, product.stock_qty_);
    sqlite3_bind_text(stmt.get(), 6, product.category_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 7, product.image_url_.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        throw std::runtime_error("Failed to insert product: " + std::string(sqlite3_errmsg(db_)));
    }

    int64_t new_id = sqlite3_last_insert_rowid(db_);
    model::Product created = product;
    created.id_ = new_id;
    return created;
}

model::Product SQLiteProductRepository::Update(const model::Product& product) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "UPDATE products SET name = ?, description = ?, price_cents = ?, stock_qty = ?, category = ?, image_url = ? "
        "WHERE id = ? AND seller_id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_text(stmt.get(), 1, product.name_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 2, product.description_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt.get(), 3, product.price_cents_.GetCents());
    sqlite3_bind_int(stmt.get(), 4, product.stock_qty_);
    sqlite3_bind_text(stmt.get(), 5, product.category_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 6, product.image_url_.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt.get(), 7, product.id_);
    sqlite3_bind_int64(stmt.get(), 8, product.seller_id_);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        throw std::runtime_error("Failed to update product: " + std::string(sqlite3_errmsg(db_)));
    }
    return product;
}

bool SQLiteProductRepository::Delete(int64_t id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "DELETE FROM products WHERE id = ?;";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, id);

    return sqlite3_step(stmt.get()) == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

bool SQLiteProductRepository::UpdateStock(int64_t id, int32_t delta) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "UPDATE products SET stock_qty = stock_qty + ? WHERE id = ? AND (stock_qty + ?) >= 0;";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int(stmt.get(), 1, delta);
    sqlite3_bind_int64(stmt.get(), 2, id);
    sqlite3_bind_int(stmt.get(), 3, delta);

    return sqlite3_step(stmt.get()) == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

int64_t SQLiteProductRepository::Count() {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "SELECT COUNT(*) FROM products;";
    auto stmt = PrepareStatement(db_, sql);
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return sqlite3_column_int64(stmt.get(), 0);
    }
    return 0;
}

} // namespace faaliha::faalihamart::repository::sqlite
