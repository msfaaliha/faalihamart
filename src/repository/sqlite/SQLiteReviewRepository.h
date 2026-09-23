#pragma once

#include "../IReviewRepository.h"
#include <sqlite3.h>
#include <mutex>

namespace faaliha::faalihamart::repository::sqlite {

/**
 * @brief SQLite implementation of IReviewRepository.
 */
class SQLiteReviewRepository : public IReviewRepository {
public:
    explicit SQLiteReviewRepository(sqlite3* db);

    std::vector<model::Review> FindByProductId(int64_t product_id) override;
    std::optional<model::Review> FindByUserAndProduct(int64_t user_id, int64_t product_id) override;
    model::Review Create(const model::Review& review) override;
    bool Delete(int64_t id) override;
    void GetRatingSummary(int64_t product_id, double& out_avg_rating, int64_t& out_count) override;

private:
    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
};

} // namespace faaliha::faalihamart::repository::sqlite
