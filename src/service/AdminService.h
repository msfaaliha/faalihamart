#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../repository/IUserRepository.h"
#include "../repository/IProductRepository.h"
#include "../repository/IOrderRepository.h"
#include "../dto/AuthDto.h"
#include "../dto/OrderDto.h"
#include "../dto/AdminDto.h"

namespace faaliha::faalihamart::service {

/**
 * @brief Admin oversight and platform analytics service.
 */
class AdminService {
public:
    AdminService(std::shared_ptr<repository::IUserRepository> user_repo,
                 std::shared_ptr<repository::IProductRepository> product_repo,
                 std::shared_ptr<repository::IOrderRepository> order_repo);

    /**
     * @brief Retrieve all users in the system (Admin only).
     */
    std::vector<dto::UserResponseDto> GetAllUsers(bool is_admin, const std::string& request_id = "");

    /**
     * @brief Retrieve all orders in the system (Admin only).
     */
    std::vector<dto::OrderResponseDto> GetAllOrders(bool is_admin, const std::string& request_id = "");

    /**
     * @brief Retrieve platform summary statistics (Admin only).
     */
    dto::AdminDashboardStatsDto GetDashboardStats(bool is_admin, const std::string& request_id = "");

    /**
     * @brief Retrieve seller sales dashboard metrics (O3 feature).
     */
    dto::SellerDashboardStatsDto GetSellerDashboardStats(int64_t seller_id, const std::string& request_id = "");

private:
    std::shared_ptr<repository::IUserRepository> user_repo_;
    std::shared_ptr<repository::IProductRepository> product_repo_;
    std::shared_ptr<repository::IOrderRepository> order_repo_;
};

} // namespace faaliha::faalihamart::service
