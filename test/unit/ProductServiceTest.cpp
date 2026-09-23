#include <gtest/gtest.h>
#include <unordered_map>
#include "service/ProductService.h"
#include "repository/IProductRepository.h"
#include "exception/ApiException.h"

using namespace faaliha::faalihamart;

class FakeProductRepository : public repository::IProductRepository {
public:
    std::optional<model::Product> FindById(int64_t id) override {
        auto it = prods_.find(id);
        if (it != prods_.end()) return it->second;
        return std::nullopt;
    }

    std::vector<model::Product> FindAll(const dto::ProductFilterDto& /*filter*/) override {
        std::vector<model::Product> res;
        for (const auto& [_, p] : prods_) res.push_back(p);
        return res;
    }

    std::vector<model::Product> FindBySellerId(int64_t seller_id) override {
        std::vector<model::Product> res;
        for (const auto& [_, p] : prods_) {
            if (p.seller_id_ == seller_id) res.push_back(p);
        }
        return res;
    }

    model::Product Create(const model::Product& product) override {
        model::Product p = product;
        p.id_ = next_id_++;
        prods_[p.id_] = p;
        return p;
    }

    model::Product Update(const model::Product& product) override {
        prods_[product.id_] = product;
        return product;
    }

    bool Delete(int64_t id) override {
        return prods_.erase(id) > 0;
    }

    bool UpdateStock(int64_t id, int32_t delta) override {
        auto it = prods_.find(id);
        if (it == prods_.end()) return false;
        if (it->second.stock_qty_ + delta < 0) return false;
        it->second.stock_qty_ += delta;
        return true;
    }

    int64_t Count() override {
        return static_cast<int64_t>(prods_.size());
    }

private:
    int64_t next_id_{1};
    std::unordered_map<int64_t, model::Product> prods_;
};

TEST(ProductServiceTest, CreateAndOwnershipCheck) {
    auto fake_repo = std::make_shared<FakeProductRepository>();
    service::ProductService prod_svc(fake_repo);

    int64_t seller_id = 2;
    dto::ProductRequestDto req;
    req.name = "Pro Mechanical Keyboard";
    req.category = "Electronics";
    req.price_cents = 9999; // $99.99
    req.stock_qty = 15;
    req.description = "Compact 75% layout.";

    auto created = prod_svc.CreateProduct(seller_id, req, "req-test");
    EXPECT_EQ(created.name, "Pro Mechanical Keyboard");
    EXPECT_EQ(created.price_cents, 9999);
    EXPECT_EQ(created.price_formatted, "₹99.99");

    // Another seller cannot update this product
    int64_t unauthorized_seller = 3;
    dto::ProductRequestDto update_req = req;
    update_req.name = "Hacked Listing";
    EXPECT_THROW(
        prod_svc.UpdateProduct(created.id, unauthorized_seller, update_req, "req-test"),
        exception::ForbiddenException
    );

    // Another seller cannot delete this product
    EXPECT_THROW(
        prod_svc.DeleteProduct(created.id, unauthorized_seller, false, "req-test"),
        exception::ForbiddenException
    );

    // Admin can delete any listing
    EXPECT_TRUE(prod_svc.DeleteProduct(created.id, 999, true, "req-test"));
}
