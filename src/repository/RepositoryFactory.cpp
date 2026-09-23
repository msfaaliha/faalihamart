#include "RepositoryFactory.h"
#include "sqlite/SQLiteUserRepository.h"
#include "sqlite/SQLiteProductRepository.h"
#include "sqlite/SQLiteCartRepository.h"
#include "sqlite/SQLiteOrderRepository.h"
#include "sqlite/SQLiteReviewRepository.h"
#include "sqlite/SQLiteMigrationRepository.h"

namespace faaliha::faalihamart::repository {

std::shared_ptr<IUserRepository> RepositoryFactory::CreateUserRepository(sqlite3* db) {
    return std::make_shared<sqlite::SQLiteUserRepository>(db);
}

std::shared_ptr<IProductRepository> RepositoryFactory::CreateProductRepository(sqlite3* db) {
    return std::make_shared<sqlite::SQLiteProductRepository>(db);
}

std::shared_ptr<ICartRepository> RepositoryFactory::CreateCartRepository(sqlite3* db) {
    return std::make_shared<sqlite::SQLiteCartRepository>(db);
}

std::shared_ptr<IOrderRepository> RepositoryFactory::CreateOrderRepository(sqlite3* db) {
    return std::make_shared<sqlite::SQLiteOrderRepository>(db);
}

std::shared_ptr<IReviewRepository> RepositoryFactory::CreateReviewRepository(sqlite3* db) {
    return std::make_shared<sqlite::SQLiteReviewRepository>(db);
}

std::shared_ptr<IMigrationRepository> RepositoryFactory::CreateMigrationRepository(sqlite3* db) {
    return std::make_shared<sqlite::SQLiteMigrationRepository>(db);
}

} // namespace faaliha::faalihamart::repository
