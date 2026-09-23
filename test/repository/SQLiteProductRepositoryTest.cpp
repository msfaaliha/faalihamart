#include <gtest/gtest.h>
#include <sqlite3.h>
#include "repository/sqlite/SQLiteProductRepository.h"

using namespace faaliha::faalihamart;

class SQLiteProductRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        sqlite3_open(":memory:", &db_);
        const char* schema = 
            "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT);"
            "INSERT INTO users (id, name) VALUES (2, 'Tech Merchant');"
            "CREATE TABLE products ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    seller_id INTEGER NOT NULL,"
            "    name TEXT NOT NULL,"
            "    description TEXT,"
            "    price_cents BIGINT NOT NULL,"
            "    stock_qty INTEGER NOT NULL DEFAULT 0,"
            "    category TEXT NOT NULL,"
            "    image_url TEXT,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ");";
        sqlite3_exec(db_, schema, nullptr, nullptr, nullptr);
        repo_ = std::make_unique<repository::sqlite::SQLiteProductRepository>(db_);
    }

    void TearDown() override {
        sqlite3_close(db_);
    }

    sqlite3* db_{nullptr};
    std::unique_ptr<repository::sqlite::SQLiteProductRepository> repo_;
};

TEST_F(SQLiteProductRepositoryTest, CreateSearchAndFilterProducts) {
    model::Product p1;
    p1.seller_id_ = 2;
    p1.name_ = "Wireless Headphones";
    p1.description_ = "High-fidelity noise cancellation.";
    p1.price_cents_ = model::Money::FromCents(14999);
    p1.stock_qty_ = 20;
    p1.category_ = "Electronics";
    p1.image_url_ = "http://example.com/img.jpg";

    auto c1 = repo_->Create(p1);
    EXPECT_GT(c1.id_, 0);

    // Search by category
    dto::ProductFilterDto filter;
    filter.category = "Electronics";
    auto results = repo_->FindAll(filter);
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].seller_name_, "Tech Merchant");

    // Search by keyword
    filter.category = std::nullopt;
    filter.keyword = "fidelity";
    results = repo_->FindAll(filter);
    EXPECT_EQ(results.size(), 1);

    // Atomic stock decrement
    EXPECT_TRUE(repo_->UpdateStock(c1.id_, -5));
    auto updated = repo_->FindById(c1.id_);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->stock_qty_, 15);

    // Cannot decrement beyond available stock
    EXPECT_FALSE(repo_->UpdateStock(c1.id_, -20));
}
