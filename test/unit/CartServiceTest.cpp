#include <gtest/gtest.h>
#include <unordered_map>
#include "service/CartService.h"
#include "repository/ICartRepository.h"
#include "repository/IProductRepository.h"
#include "exception/ApiException.h"

using namespace faaliha::faalihamart;

class FakeCartRepository : public repository::ICartRepository {
public:
    std::vector<model::CartItem> FindByUserId(int64_t user_id) override {
        std::vector<model::CartItem> res;
        for (const auto& [_, it] : items_) {
            if (it.user_id_ == user_id) res.push_back(it);
        }
        return res;
    }

    std::optional<model::CartItem> FindById(int64_t id) override {
        auto it = items_.find(id);
        if (it != items_.end()) return it->second;
        return std::nullopt;
    }

    std::optional<model::CartItem> FindByUserAndProduct(int64_t user_id, int64_t product_id) override {
        for (const auto& [_, it] : items_) {
            if (it.user_id_ == user_id && it.product_id_ == product_id) return it;
        }
        return std::nullopt;
    }

    model::CartItem Save(const model::CartItem& item) override {
        model::CartItem ci = item;
        if (ci.id_ == 0) ci.id_ = next_id_++;
        items_[ci.id_] = ci;
        return ci;
    }

    bool Delete(int64_t id, int64_t /*user_id*/) override {
        return items_.erase(id) > 0;
    }

    bool ClearCart(int64_t user_id) override {
        for (auto it = items_.begin(); it != items_.end();) {
            if (it->second.user_id_ == user_id) it = items_.erase(it);
            else ++it;
        }
        return true;
    }

private:
    int64_t next_id_{1};
    std::unordered_map<int64_t, model::CartItem> items_;
};

class FakeProdRepoForCart : public repository::IProductRepository {
public:
    std::optional<model::Product> FindById(int64_t id) override {
        if (id == 1) {
            model::Product p;
            p.id_ = 1;
            p.name_ = "Wireless Headphones";
            p.price_cents_ = model::Money::FromCents(10000); // $100.00
            p.stock_qty_ = 5;
            return p;
        }
        return std::nullopt;
    }
    std::vector<model::Product> FindAll(const dto::ProductFilterDto&) override { return {}; }
    std::vector<model::Product> FindBySellerId(int64_t) override { return {}; }
    model::Product Create(const model::Product& p) override { return p; }
    model::Product Update(const model::Product& p) override { return p; }
    bool Delete(int64_t) override { return true; }
    bool UpdateStock(int64_t, int32_t) override { return true; }
    int64_t Count() override { return 1; }
};

TEST(CartServiceTest, AddItemAndExceedStockValidation) {
    auto cart_repo = std::make_shared<FakeCartRepository>();
    auto prod_repo = std::make_shared<FakeProdRepoForCart>();
    service::CartService cart_svc(cart_repo, prod_repo);

    int64_t buyer_id = 4;
    dto::AddToCartRequestDto req;
    req.product_id = 1;
    req.quantity = 2;

    auto cart = cart_svc.AddItem(buyer_id, req, "req-test");
    EXPECT_EQ(cart.total_items, 2);

    // Attempting to exceed stock (stock is 5, already have 2, adding 4 would exceed 5)
    dto::AddToCartRequestDto excessive_req;
    excessive_req.product_id = 1;
    excessive_req.quantity = 4;
    EXPECT_THROW(cart_svc.AddItem(buyer_id, excessive_req, "req-test"), exception::ValidationException);
}
