#pragma once

#include "../ICartRepository.h"
#include <sqlite3.h>
#include <mutex>

namespace faaliha::faalihamart::repository::sqlite {

/**
 * @brief SQLite implementation of ICartRepository.
 */
class SQLiteCartRepository : public ICartRepository {
public:
    explicit SQLiteCartRepository(sqlite3* db);

    std::vector<model::CartItem> FindByUserId(int64_t user_id) override;
    std::optional<model::CartItem> FindById(int64_t id) override;
    std::optional<model::CartItem> FindByUserAndProduct(int64_t user_id, int64_t product_id) override;
    model::CartItem Save(const model::CartItem& item) override;
    bool Delete(int64_t id, int64_t user_id) override;
    bool ClearCart(int64_t user_id) override;

private:
    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
};

} // namespace faaliha::faalihamart::repository::sqlite
