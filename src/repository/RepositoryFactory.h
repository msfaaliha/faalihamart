#pragma once

#include <memory>
#include <sqlite3.h>
#include "IUserRepository.h"
#include "IProductRepository.h"
#include "ICartRepository.h"
#include "IOrderRepository.h"
#include "IReviewRepository.h"
#include "IMigrationRepository.h"

namespace faaliha::faalihamart::repository {

/**
 * @brief Factory for instantiating repository implementations.
 */
class RepositoryFactory {
public:
    static std::shared_ptr<IUserRepository> CreateUserRepository(sqlite3* db);
    static std::shared_ptr<IProductRepository> CreateProductRepository(sqlite3* db);
    static std::shared_ptr<ICartRepository> CreateCartRepository(sqlite3* db);
    static std::shared_ptr<IOrderRepository> CreateOrderRepository(sqlite3* db);
    static std::shared_ptr<IReviewRepository> CreateReviewRepository(sqlite3* db);
    static std::shared_ptr<IMigrationRepository> CreateMigrationRepository(sqlite3* db);
};

} // namespace faaliha::faalihamart::repository
