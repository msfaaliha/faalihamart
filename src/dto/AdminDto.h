#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "AuthDto.h"
#include "OrderDto.h"
#include "ProductDto.h"

namespace faaliha::faalihamart::dto {

class AdminDashboardStatsDto {
public:
    int64_t total_users{0};
    int64_t total_sellers{0};
    int64_t total_buyers{0};
    int64_t total_products{0};
    int64_t total_orders{0};
    int64_t total_revenue_cents{0};
    std::string total_revenue_formatted;

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"total_users", total_users},
            {"total_sellers", total_sellers},
            {"total_buyers", total_buyers},
            {"total_products", total_products},
            {"total_orders", total_orders},
            {"total_revenue_cents", total_revenue_cents},
            {"total_revenue_formatted", total_revenue_formatted}
        };
    }
};

class SellerDashboardStatsDto {
public:
    int64_t total_listings{0};
    int64_t active_orders{0};
    int64_t completed_orders{0};
    int64_t total_revenue_cents{0};
    std::string total_revenue_formatted;

    [[nodiscard]] nlohmann::json ToJson() const {
        return {
            {"total_listings", total_listings},
            {"active_orders", active_orders},
            {"completed_orders", completed_orders},
            {"total_revenue_cents", total_revenue_cents},
            {"total_revenue_formatted", total_revenue_formatted}
        };
    }
};

} // namespace faaliha::faalihamart::dto
