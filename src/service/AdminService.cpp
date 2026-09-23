#include "AdminService.h"
#include "../exception/ApiException.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::service {

AdminService::AdminService(std::shared_ptr<repository::IUserRepository> user_repo,
                           std::shared_ptr<repository::IProductRepository> product_repo,
                           std::shared_ptr<repository::IOrderRepository> order_repo)
    : user_repo_(std::move(user_repo)),
      product_repo_(std::move(product_repo)),
      order_repo_(std::move(order_repo)) {}

std::vector<dto::UserResponseDto> AdminService::GetAllUsers(bool is_admin, const std::string& request_id) {
    spdlog::info("[{}] AdminService::GetAllUsers: is_admin={}", request_id, is_admin);
    if (!is_admin) {
        throw exception::ForbiddenException("Admin access required");
    }

    auto users = user_repo_->FindAll();
    std::vector<dto::UserResponseDto> dtos;
    dtos.reserve(users.size());
    for (const auto& u : users) {
        dtos.push_back(dto::UserResponseDto::FromEntity(u));
    }
    return dtos;
}

std::vector<dto::OrderResponseDto> AdminService::GetAllOrders(bool is_admin, const std::string& request_id) {
    spdlog::info("[{}] AdminService::GetAllOrders: is_admin={}", request_id, is_admin);
    if (!is_admin) {
        throw exception::ForbiddenException("Admin access required");
    }

    auto orders = order_repo_->FindAll();
    std::vector<dto::OrderResponseDto> dtos;
    dtos.reserve(orders.size());
    for (const auto& ord : orders) {
        dtos.push_back(dto::OrderResponseDto::FromEntity(ord));
    }
    return dtos;
}

dto::AdminDashboardStatsDto AdminService::GetDashboardStats(bool is_admin, const std::string& request_id) {
    spdlog::info("[{}] AdminService::GetDashboardStats: is_admin={}", request_id, is_admin);
    if (!is_admin) {
        throw exception::ForbiddenException("Admin access required");
    }

    dto::AdminDashboardStatsDto stats;
    stats.total_users = user_repo_->Count();
    stats.total_sellers = user_repo_->Count(model::UserRole::SELLER);
    stats.total_buyers = user_repo_->Count(model::UserRole::BUYER);
    stats.total_products = product_repo_->Count();

    int64_t total_orders = 0;
    int64_t total_revenue_cents = 0;
    order_repo_->GetPlatformStats(total_orders, total_revenue_cents);

    stats.total_orders = total_orders;
    stats.total_revenue_cents = total_revenue_cents;
    stats.total_revenue_formatted = model::Money::FromCents(total_revenue_cents).ToString();

    return stats;
}

dto::SellerDashboardStatsDto AdminService::GetSellerDashboardStats(int64_t seller_id, const std::string& request_id) {
    spdlog::info("[{}] AdminService::GetSellerDashboardStats: seller_id={}", request_id, seller_id);

    dto::SellerDashboardStatsDto stats;
    auto listings = product_repo_->FindBySellerId(seller_id);
    stats.total_listings = listings.size();

    int64_t order_count = 0;
    int64_t revenue_cents = 0;
    order_repo_->GetSellerStats(seller_id, order_count, revenue_cents);

    auto orders = order_repo_->FindBySellerId(seller_id);
    int64_t active_orders = 0;
    int64_t completed_orders = 0;
    for (const auto& o : orders) {
        if (o.status_ == model::OrderStatus::DELIVERED) {
            completed_orders++;
        } else if (o.status_ != model::OrderStatus::CANCELLED) {
            active_orders++;
        }
    }

    stats.active_orders = active_orders;
    stats.completed_orders = completed_orders;
    stats.total_revenue_cents = revenue_cents;
    stats.total_revenue_formatted = model::Money::FromCents(revenue_cents).ToString();

    return stats;
}

} // namespace faaliha::faalihamart::service
