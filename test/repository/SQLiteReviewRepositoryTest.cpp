#include <gtest/gtest.h>
#include <sqlite3.h>
#include "repository/sqlite/SQLiteReviewRepository.h"

using namespace faaliha::faalihamart;

class SQLiteReviewRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        sqlite3_open(":memory:", &db_);
        const char* schema = 
            "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT);"
            "INSERT INTO users (id, name) VALUES (1, 'Alice');"
            "CREATE TABLE products (id INTEGER PRIMARY KEY, name TEXT);"
            "INSERT INTO products (id, name) VALUES (1, 'Headphones');"
            "CREATE TABLE orders (id INTEGER PRIMARY KEY, buyer_id INTEGER, status TEXT);"
            "INSERT INTO orders (id, buyer_id, status) VALUES (1, 1, 'DELIVERED');"
            "CREATE TABLE order_items (id INTEGER PRIMARY KEY, order_id INTEGER, product_id INTEGER, quantity INTEGER, unit_price_cents BIGINT);"
            "INSERT INTO order_items (id, order_id, product_id, quantity, unit_price_cents) VALUES (1, 1, 1, 1, 14999);"
            "CREATE TABLE reviews ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    product_id INTEGER NOT NULL,"
            "    user_id INTEGER NOT NULL,"
            "    rating INTEGER NOT NULL,"
            "    comment TEXT,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ");";
        sqlite3_exec(db_, schema, nullptr, nullptr, nullptr);
        repo_ = std::make_unique<repository::sqlite::SQLiteReviewRepository>(db_);
    }

    void TearDown() override {
        sqlite3_close(db_);
    }

    sqlite3* db_{nullptr};
    std::unique_ptr<repository::sqlite::SQLiteReviewRepository> repo_;
};

TEST_F(SQLiteReviewRepositoryTest, PurchaseVerificationAndReviewCreation) {
    // Verified delivered order exists for User 1 and Product 1
    EXPECT_TRUE(repo_->HasUserPurchasedAndDelivered(1, 1));

    // User 99 has no orders
    EXPECT_FALSE(repo_->HasUserPurchasedAndDelivered(99, 1));

    // Create review
    model::Review r;
    r.product_id_ = 1;
    r.user_id_ = 1;
    r.rating_ = 5;
    r.comment_ = "Exceptional audio clarity and battery life.";

    auto created = repo_->Create(r);
    EXPECT_GT(created.id_, 0);

    auto list = repo_->FindByProductId(1);
    ASSERT_EQ(list.size(), 1);
    EXPECT_EQ(list[0].user_name_, "Alice");
    EXPECT_EQ(list[0].rating_, 5);

    auto [avg, count] = repo_->GetAverageRatingAndCount(1);
    EXPECT_DOUBLE_EQ(avg, 5.0);
    EXPECT_EQ(count, 1);
}
