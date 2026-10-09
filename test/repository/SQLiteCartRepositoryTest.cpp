#include <gtest/gtest.h>
#include <sqlite3.h>
#include "repository/sqlite/SQLiteCartRepository.h"

using namespace faaliha::faalihamart;

class SQLiteCartRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        sqlite3_open(":memory:", &db_);
        const char* schema = 
            "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT);"
            "INSERT INTO users (id, name) VALUES (1, 'Alice');"
            "CREATE TABLE products ("
            "    id INTEGER PRIMARY KEY,"
            "    seller_id INTEGER NOT NULL,"
            "    name TEXT NOT NULL,"
            "    price_cents BIGINT NOT NULL,"
            "    stock_qty INTEGER NOT NULL DEFAULT 10,"
            "    image_url TEXT"
            ");"
            "INSERT INTO products (id, seller_id, name, price_cents, stock_qty) VALUES (1, 1, 'Headphones', 14999, 10);"
            "CREATE TABLE cart_items ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    user_id INTEGER NOT NULL,"
            "    product_id INTEGER NOT NULL,"
            "    quantity INTEGER NOT NULL DEFAULT 1,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "    UNIQUE(user_id, product_id)"
            ");";
        sqlite3_exec(db_, schema, nullptr, nullptr, nullptr);
        repo_ = std::make_unique<repository::sqlite::SQLiteCartRepository>(db_);
    }

    void TearDown() override {
        sqlite3_close(db_);
    }

    sqlite3* db_{nullptr};
    std::unique_ptr<repository::sqlite::SQLiteCartRepository> repo_;
};

TEST_F(SQLiteCartRepositoryTest, AddUpdateAndClearCartItems) {
    // 1. Add item
    auto item = repo_->AddOrUpdateItem(1, 1, 2);
    EXPECT_GT(item.id_, 0);
    EXPECT_EQ(item.quantity_, 2);

    auto items = repo_->FindByUserId(1);
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items[0].product_name_, "Headphones");
    EXPECT_EQ(items[0].product_price_cents_.GetCents(), 14999);

    // 2. Update quantity
    EXPECT_TRUE(repo_->UpdateQuantity(item.id_, 1, 5));
    items = repo_->FindByUserId(1);
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items[0].quantity_, 5);

    // 3. Clear cart
    EXPECT_TRUE(repo_->ClearByUserId(1));
    items = repo_->FindByUserId(1);
    EXPECT_TRUE(items.empty());
}
