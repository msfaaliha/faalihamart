#include <gtest/gtest.h>
#include <sqlite3.h>
#include "repository/sqlite/SQLiteOrderRepository.h"

using namespace faaliha::faalihamart;

class SQLiteOrderRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        sqlite3_open(":memory:", &db_);
        const char* schema = 
            "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT);"
            "INSERT INTO users (id, name, email) VALUES (1, 'Alice', 'alice@test.com');"
            "INSERT INTO users (id, name, email) VALUES (2, 'Seller Tech', 'seller@test.com');"
            "CREATE TABLE products ("
            "    id INTEGER PRIMARY KEY,"
            "    seller_id INTEGER NOT NULL,"
            "    name TEXT NOT NULL,"
            "    price_cents BIGINT NOT NULL,"
            "    stock_qty INTEGER NOT NULL DEFAULT 10,"
            "    category TEXT NOT NULL DEFAULT 'Electronics'"
            ");"
            "INSERT INTO products (id, seller_id, name, price_cents, stock_qty) VALUES (1, 2, 'Headphones', 10000, 10);"
            "CREATE TABLE cart_items ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    user_id INTEGER NOT NULL,"
            "    product_id INTEGER NOT NULL,"
            "    quantity INTEGER NOT NULL DEFAULT 1,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "    UNIQUE(user_id, product_id)"
            ");"
            "INSERT INTO cart_items (user_id, product_id, quantity) VALUES (1, 1, 2);"
            "CREATE TABLE orders ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    buyer_id INTEGER NOT NULL,"
            "    status TEXT NOT NULL,"
            "    total_amount_cents BIGINT NOT NULL,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ");"
            "CREATE TABLE order_items ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    order_id INTEGER NOT NULL,"
            "    product_id INTEGER NOT NULL,"
            "    quantity INTEGER NOT NULL,"
            "    unit_price_cents BIGINT NOT NULL"
            ");";
        sqlite3_exec(db_, schema, nullptr, nullptr, nullptr);
        repo_ = std::make_unique<repository::sqlite::SQLiteOrderRepository>(db_);
    }

    void TearDown() override {
        sqlite3_close(db_);
    }

    sqlite3* db_{nullptr};
    std::unique_ptr<repository::sqlite::SQLiteOrderRepository> repo_;
};

TEST_F(SQLiteOrderRepositoryTest, CreateOrderAtomicallyAndVerifyStatusWorkflow) {
    model::CartItem item;
    item.product_id_ = 1;
    item.quantity_ = 2;
    item.product_price_cents_ = model::Money::FromCents(10000);

    model::Money total = model::Money::FromCents(20000);
    auto order = repo_->CreateOrderWithItems(1, {item}, total);

    EXPECT_GT(order.id_, 0);
    EXPECT_EQ(order.buyer_id_, 1);
    EXPECT_EQ(order.status_, model::OrderStatus::PENDING);
    EXPECT_EQ(order.total_amount_cents_.GetCents(), 20000);

    // Verify buyer orders
    auto buyer_orders = repo_->FindByBuyerId(1);
    ASSERT_EQ(buyer_orders.size(), 1);
    EXPECT_EQ(buyer_orders[0].id_, order.id_);

    // Verify seller orders
    auto seller_orders = repo_->FindBySellerId(2);
    ASSERT_EQ(seller_orders.size(), 1);
    EXPECT_EQ(seller_orders[0].id_, order.id_);

    // Status progression: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED
    EXPECT_TRUE(repo_->UpdateStatus(order.id_, model::OrderStatus::CONFIRMED));
    auto o_confirmed = repo_->FindById(order.id_);
    ASSERT_TRUE(o_confirmed.has_value());
    EXPECT_EQ(o_confirmed->status_, model::OrderStatus::CONFIRMED);

    EXPECT_TRUE(repo_->UpdateStatus(order.id_, model::OrderStatus::DELIVERED));
    auto o_delivered = repo_->FindById(order.id_);
    ASSERT_TRUE(o_delivered.has_value());
    EXPECT_EQ(o_delivered->status_, model::OrderStatus::DELIVERED);

    // Platform revenue calculation
    auto rev = repo_->CalculateTotalPlatformRevenue();
    EXPECT_EQ(rev.GetCents(), 20000);
}
