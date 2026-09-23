#pragma once

#include "../IProductRepository.h"
#include <sqlite3.h>
#include <mutex>

namespace faaliha::faalihamart::repository::sqlite {

/**
 * @brief SQLite implementation of IProductRepository using parameterized queries.
 */
class SQLiteProductRepository : public IProductRepository {
public:
    explicit SQLiteProductRepository(sqlite3* db);

    std::optional<model::Product> FindById(int64_t id) override;
    std::vector<model::Product> FindAll(const dto::ProductFilterDto& filter) override;
    std::vector<model::Product> FindBySellerId(int64_t seller_id) override;
    model::Product Create(const model::Product& product) override;
    model::Product Update(const model::Product& product) override;
    bool Delete(int64_t id) override;
    bool UpdateStock(int64_t id, int32_t delta) override;
    int64_t Count() override;

private:
    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
};

} // namespace faaliha::faalihamart::repository::sqlite
