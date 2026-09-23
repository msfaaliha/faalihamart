#include "ProductService.h"
#include "../util/ValidationUtil.h"
#include "../exception/ApiException.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::service {

ProductService::ProductService(std::shared_ptr<repository::IProductRepository> product_repo)
    : product_repo_(std::move(product_repo)) {}

dto::ProductResponseDto ProductService::GetProductById(int64_t id, const std::string& request_id) {
    spdlog::info("[{}] ProductService::GetProductById: id={}", request_id, id);
    auto prod_opt = product_repo_->FindById(id);
    if (!prod_opt.has_value()) {
        throw exception::NotFoundException("Product not found with ID: " + std::to_string(id));
    }
    return dto::ProductResponseDto::FromEntity(*prod_opt);
}

std::vector<dto::ProductResponseDto> ProductService::SearchProducts(const dto::ProductFilterDto& filter, const std::string& request_id) {
    spdlog::info("[{}] ProductService::SearchProducts: category={}, keyword={}",
                 request_id, filter.category.value_or("ALL"), filter.keyword.value_or(""));

    auto prods = product_repo_->FindAll(filter);
    std::vector<dto::ProductResponseDto> dtos;
    dtos.reserve(prods.size());
    for (const auto& p : prods) {
        dtos.push_back(dto::ProductResponseDto::FromEntity(p));
    }
    return dtos;
}

std::vector<dto::ProductResponseDto> ProductService::GetSellerProducts(int64_t seller_id, const std::string& request_id) {
    spdlog::info("[{}] ProductService::GetSellerProducts: seller_id={}", request_id, seller_id);
    auto prods = product_repo_->FindBySellerId(seller_id);
    std::vector<dto::ProductResponseDto> dtos;
    dtos.reserve(prods.size());
    for (const auto& p : prods) {
        dtos.push_back(dto::ProductResponseDto::FromEntity(p));
    }
    return dtos;
}

dto::ProductResponseDto ProductService::CreateProduct(int64_t seller_id, const dto::ProductRequestDto& req, const std::string& request_id) {
    spdlog::info("[{}] ProductService::CreateProduct: seller_id={}, name={}", request_id, seller_id, req.name);

    // Validation at top of method
    util::ValidationUtil::ValidateNonEmpty("name", req.name);
    util::ValidationUtil::ValidateNonEmpty("category", req.category);
    util::ValidationUtil::ValidatePositive("price_cents", req.price_cents);
    util::ValidationUtil::ValidateNonNegative("stock_qty", req.stock_qty);

    model::Product p;
    p.seller_id_ = seller_id;
    p.name_ = req.name;
    p.description_ = req.description;
    p.price_cents_ = model::Money::FromCents(req.price_cents);
    p.stock_qty_ = req.stock_qty;
    p.category_ = req.category;
    p.image_url_ = req.image_url.empty() ? 
        "https://images.unsplash.com/photo-1523275335684-37898b6baf30?w=500&q=80" : req.image_url;

    model::Product created = product_repo_->Create(p);
    return GetProductById(created.id_, request_id);
}

dto::ProductResponseDto ProductService::UpdateProduct(int64_t product_id, int64_t seller_id, const dto::ProductRequestDto& req, const std::string& request_id) {
    spdlog::info("[{}] ProductService::UpdateProduct: product_id={}, seller_id={}", request_id, product_id, seller_id);

    // Validate inputs
    util::ValidationUtil::ValidateNonEmpty("name", req.name);
    util::ValidationUtil::ValidateNonEmpty("category", req.category);
    util::ValidationUtil::ValidatePositive("price_cents", req.price_cents);
    util::ValidationUtil::ValidateNonNegative("stock_qty", req.stock_qty);

    auto existing = product_repo_->FindById(product_id);
    if (!existing.has_value()) {
        throw exception::NotFoundException("Product not found");
    }

    // Enforce ownership
    if (existing->seller_id_ != seller_id) {
        spdlog::warn("[{}] Unauthorized attempt by user {} to edit product {}", request_id, seller_id, product_id);
        throw exception::ForbiddenException("You can only edit your own listings");
    }

    model::Product updated = *existing;
    updated.name_ = req.name;
    updated.description_ = req.description;
    updated.price_cents_ = model::Money::FromCents(req.price_cents);
    updated.stock_qty_ = req.stock_qty;
    updated.category_ = req.category;
    if (!req.image_url.empty()) {
        updated.image_url_ = req.image_url;
    }

    product_repo_->Update(updated);
    return GetProductById(product_id, request_id);
}

bool ProductService::DeleteProduct(int64_t product_id, int64_t requesting_user_id, bool is_admin, const std::string& request_id) {
    spdlog::info("[{}] ProductService::DeleteProduct: product_id={}, user_id={}, is_admin={}",
                 request_id, product_id, requesting_user_id, is_admin);

    auto existing = product_repo_->FindById(product_id);
    if (!existing.has_value()) {
        throw exception::NotFoundException("Product not found");
    }

    if (!is_admin && existing->seller_id_ != requesting_user_id) {
        spdlog::warn("[{}] Unauthorized attempt to delete product {}", request_id, product_id);
        throw exception::ForbiddenException("You do not have permission to delete this listing");
    }

    return product_repo_->Delete(product_id);
}

} // namespace faaliha::faalihamart::service
