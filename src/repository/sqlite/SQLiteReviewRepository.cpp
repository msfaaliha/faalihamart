#include "SQLiteReviewRepository.h"
#include "SQLiteHelper.h"
#include <stdexcept>

namespace faaliha::faalihamart::repository::sqlite {

SQLiteReviewRepository::SQLiteReviewRepository(sqlite3* db) : db_(db) {}

std::vector<model::Review> SQLiteReviewRepository::FindByProductId(int64_t product_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT r.id, r.product_id, r.user_id, r.rating, r.comment, r.created_at, u.name as user_name "
        "FROM reviews r "
        "JOIN users u ON r.user_id = u.id "
        "WHERE r.product_id = ? "
        "ORDER BY r.id DESC;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, product_id);

    std::vector<model::Review> reviews;
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Review r;
        r.id_ = sqlite3_column_int64(stmt.get(), 0);
        r.product_id_ = sqlite3_column_int64(stmt.get(), 1);
        r.user_id_ = sqlite3_column_int64(stmt.get(), 2);
        r.rating_ = sqlite3_column_int(stmt.get(), 3);
        const char* comm = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        r.comment_ = comm ? comm : "";
        r.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        r.user_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        reviews.push_back(r);
    }
    return reviews;
}

std::optional<model::Review> SQLiteReviewRepository::FindByUserAndProduct(int64_t user_id, int64_t product_id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "SELECT r.id, r.product_id, r.user_id, r.rating, r.comment, r.created_at, u.name as user_name "
        "FROM reviews r "
        "JOIN users u ON r.user_id = u.id "
        "WHERE r.user_id = ? AND r.product_id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, user_id);
    sqlite3_bind_int64(stmt.get(), 2, product_id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        model::Review r;
        r.id_ = sqlite3_column_int64(stmt.get(), 0);
        r.product_id_ = sqlite3_column_int64(stmt.get(), 1);
        r.user_id_ = sqlite3_column_int64(stmt.get(), 2);
        r.rating_ = sqlite3_column_int(stmt.get(), 3);
        const char* comm = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 4));
        r.comment_ = comm ? comm : "";
        r.created_at_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 5));
        r.user_name_ = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 6));
        return r;
    }
    return std::nullopt;
}

model::Review SQLiteReviewRepository::Create(const model::Review& review) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = 
        "INSERT INTO reviews (product_id, user_id, rating, comment) VALUES (?, ?, ?, ?);";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, review.product_id_);
    sqlite3_bind_int64(stmt.get(), 2, review.user_id_);
    sqlite3_bind_int(stmt.get(), 3, review.rating_);
    sqlite3_bind_text(stmt.get(), 4, review.comment_.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        throw std::runtime_error("Failed to insert review: " + std::string(sqlite3_errmsg(db_)));
    }

    int64_t new_id = sqlite3_last_insert_rowid(db_);
    model::Review created = review;
    created.id_ = new_id;
    return created;
}

bool SQLiteReviewRepository::Delete(int64_t id) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    const std::string sql = "DELETE FROM reviews WHERE id = ?;";
    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, id);

    return sqlite3_step(stmt.get()) == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

void SQLiteReviewRepository::GetRatingSummary(int64_t product_id, double& out_avg_rating, int64_t& out_count) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    out_avg_rating = 0.0;
    out_count = 0;

    const std::string sql = 
        "SELECT COALESCE(AVG(rating), 0.0), COUNT(id) FROM reviews WHERE product_id = ?;";

    auto stmt = PrepareStatement(db_, sql);
    sqlite3_bind_int64(stmt.get(), 1, product_id);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        out_avg_rating = sqlite3_column_double(stmt.get(), 0);
        out_count = sqlite3_column_int64(stmt.get(), 1);
    }
}

} // namespace faaliha::faalihamart::repository::sqlite
