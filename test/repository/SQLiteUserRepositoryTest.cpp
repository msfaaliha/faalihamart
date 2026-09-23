#include <gtest/gtest.h>
#include <sqlite3.h>
#include "repository/sqlite/SQLiteUserRepository.h"

using namespace faaliha::faalihamart;

class SQLiteUserRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        sqlite3_open(":memory:", &db_);
        const char* schema = 
            "CREATE TABLE users ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    name TEXT NOT NULL,"
            "    email TEXT NOT NULL UNIQUE,"
            "    password_hash TEXT NOT NULL,"
            "    role TEXT NOT NULL,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ");";
        sqlite3_exec(db_, schema, nullptr, nullptr, nullptr);
        repo_ = std::make_unique<repository::sqlite::SQLiteUserRepository>(db_);
    }

    void TearDown() override {
        sqlite3_close(db_);
    }

    sqlite3* db_{nullptr};
    std::unique_ptr<repository::sqlite::SQLiteUserRepository> repo_;
};

TEST_F(SQLiteUserRepositoryTest, CreateAndFindUserParameterized) {
    model::User u;
    u.name_ = "Alice Smith";
    u.email_ = "alice@faalihamart.com";
    u.password_hash_ = "$argon2id$...";
    u.role_ = model::UserRole::BUYER;

    auto created = repo_->Create(u);
    EXPECT_GT(created.id_, 0);

    auto found = repo_->FindByEmail("alice@faalihamart.com");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name_, "Alice Smith");
    EXPECT_EQ(found->role_, model::UserRole::BUYER);

    EXPECT_EQ(repo_->Count(), 1);
    EXPECT_EQ(repo_->Count(model::UserRole::BUYER), 1);
    EXPECT_EQ(repo_->Count(model::UserRole::SELLER), 0);
}
