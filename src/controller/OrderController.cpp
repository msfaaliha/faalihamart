#include "OrderController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../service/payment/MockPaymentStrategy.h"
#include "../exception/ApiException.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::unique_ptr<service::OrderService> OrderController::GetOrderService() {
    sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
    auto order_repo = repository::RepositoryFactory::CreateOrderRepository(db);
    auto cart_repo = repository::RepositoryFactory::CreateCartRepository(db);
    auto payment_strat = std::make_shared<service::payment::MockPaymentStrategy>();
    return std::make_unique<service::OrderService>(order_repo, cart_repo, payment_strat);
}

void OrderController::Checkout(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Please login to proceed with checkout");
        }

        auto json_val = req->getJsonObject();
        dto::CheckoutRequestDto dto;
        if (json_val) {
            dto.shipping_address = json_val->get("shipping_address", "").asString();
            dto.payment_method = json_val->get("payment_method", "MOCK_CARD").asString();
        }

        auto svc = GetOrderService();
        auto order = svc->Checkout(user_id, dto, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(order.ToJson()), drogon::k201Created));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void OrderController::GetBuyerOrders(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Authentication required");
        }

        auto svc = GetOrderService();
        auto orders = svc->GetBuyerOrders(user_id, req_id);

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& o : orders) {
            arr.push_back(o.ToJson());
        }

        callback(CreateJsonResponse(dto::ApiResponse::Success(arr), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void OrderController::GetSellerOrders(const drogon::HttpRequestPtr& req,
                                      std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        std::string role = GetCurrentUserRole(req);
        if (user_id <= 0 || (role != "SELLER" && role != "ADMIN")) {
            throw exception::ForbiddenException("Seller access required");
        }

        auto svc = GetOrderService();
        auto orders = svc->GetSellerOrders(user_id, req_id);

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& o : orders) {
            arr.push_back(o.ToJson());
        }

        callback(CreateJsonResponse(dto::ApiResponse::Success(arr), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void OrderController::GetOrder(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                               int64_t id) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        std::string role = GetCurrentUserRole(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Authentication required");
        }

        bool is_admin = (role == "ADMIN");
        auto svc = GetOrderService();
        auto order = svc->GetOrderById(id, user_id, is_admin, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(order.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void OrderController::UpdateStatus(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                   int64_t id) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        std::string role = GetCurrentUserRole(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Authentication required");
        }

        auto json_val = req->getJsonObject();
        if (!json_val || !json_val->isMember("status")) {
            throw exception::ValidationException("Missing 'status' in request body");
        }

        std::string new_status = (*json_val)["status"].asString();
        bool is_admin = (role == "ADMIN");

        auto svc = GetOrderService();
        auto updated = svc->UpdateOrderStatus(id, new_status, user_id, is_admin, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(updated.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
