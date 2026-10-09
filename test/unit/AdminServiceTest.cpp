#include <gtest/gtest.h>
#include "service/AdminService.h"
#include "repository/IUserRepository.h"
#include "repository/IProductRepository.h"
#include "repository/IOrderRepository.h"
#include "exception/ApiException.h"

using namespace faaliha::faalihamart;

class FakeUserRepoForAdmin : public repository::IUserRepository {
public:
    std::optional<model::User> FindById(int64_t) override { return std::nullopt; }
    std::optional<model::User> FindByEmail(const std::string&) override { return std::nullopt; }
    model::User Create(const model::User& u) override { return u; }
    std::vector<model::User> FindAll() override {
        model::User u1; u1.id_ = 1; u1.name_ = "Admin"; u1.role_ = model::UserRole::ADMIN;
        model::User u2; u2.id_ = 2; u2.name_ = "Seller"; u2.role_ = model::UserRole::SELLER;
        return {u1, u2};
    }
    int64_t Count() override { return 2; }
    int64_t Count(model::UserRole role) override {
        if (role == model::UserRole::SELLER) return 1;
        if (role == model::UserRole::BUYER) return 0;
        return 1;
    }
};

class FakeProductRepoForAdmin : public repository::IProductRepository {
public:
    std::optional<model::Product> FindById(int64_t) override { return std::nullopt; }
    std::vector<model::Product> FindAll(const dto::ProductFilterDto&) override { return {}; }
    std::vector<model::Product> FindBySellerId(int64_t) override {
        model::Product p; p.id_ = 1; p.seller_id_ = 2;
        return {p};
    }
    model::Product Create(const model::Product& p) override { return p; }
    model::Product Update(const model::Product& p) override { return p; }
    bool Delete(int64_t) override { return true; }
    bool UpdateStock(int64_t, int32_t) override { return true; }
    int64_t Count() override { return 10; }
};

class FakeOrderRepoForAdmin : public repository::IOrderRepository {
public:
    std::optional<model::Order> FindById(int64_t) override { return std::nullopt; }
    std::vector<model::Order> FindByBuyerId(int64_t) override { return {}; }
    std::vector<model::Order> FindBySellerId(int64_t) override { return {}; }
    std::vector<model::Order> FindAll() override {
        model::Order o; o.id_ = 1; o.status_ = model::OrderStatus::DELIVERED;
        return {o};
    }
    model::Order CreateOrderWithItems(int64_t, const std::vector<model::CartItem>&, const model::Money&) override { return {}; }
    bool UpdateStatus(int64_t, model::OrderStatus) override { return true; }
    int64_t Count() override { return 5; }
    model::Money CalculateTotalPlatformRevenue() override { return model::Money::FromCents(50000); }
    model::Money CalculateSellerRevenue(int64_t) override { return model::Money::FromCents(25000); }
    int64_t CountSellerOrders(int64_t) override { return 3; }
};

TEST(AdminServiceTest, EnforcesAdminAuthorization) {
    auto user_repo = std::make_shared<FakeUserRepoForAdmin>();
    auto prod_repo = std::make_shared<FakeProductRepoForAdmin>();
    auto order_repo = std::make_shared<FakeOrderRepoForAdmin>();
    service::AdminService admin_svc(user_repo, prod_repo, order_repo);

    // Non-admin call throws ForbiddenException
    EXPECT_THROW(
        admin_svc.GetAllUsers(false, "req-adm-1"),
        exception::ForbiddenException
    );

    EXPECT_THROW(
        admin_svc.GetAllOrders(false, "req-adm-2"),
        exception::ForbiddenException
    );

    EXPECT_THROW(
        admin_svc.GetDashboardStats(false, "req-adm-3"),
        exception::ForbiddenException
    );

    // Admin call succeeds
    auto stats = admin_svc.GetDashboardStats(true, "req-adm-4");
    EXPECT_EQ(stats.total_users, 2);
    EXPECT_EQ(stats.total_sellers, 1);
    EXPECT_EQ(stats.total_products, 10);
    EXPECT_EQ(stats.total_orders, 5);
    EXPECT_EQ(stats.total_revenue_cents, 50000);
}

TEST(AdminServiceTest, ComputesSellerDashboardStats) {
    auto user_repo = std::make_shared<FakeUserRepoForAdmin>();
    auto prod_repo = std::make_shared<FakeProductRepoForAdmin>();
    auto order_repo = std::make_shared<FakeOrderRepoForAdmin>();
    service::AdminService admin_svc(user_repo, prod_repo, order_repo);

    auto seller_stats = admin_svc.GetSellerDashboardStats(2, "req-adm-5");
    EXPECT_EQ(seller_stats.seller_id, 2);
    EXPECT_EQ(seller_stats.active_listings, 1);
    EXPECT_EQ(seller_stats.total_orders, 3);
    EXPECT_EQ(seller_stats.total_revenue_cents, 25000);
}
