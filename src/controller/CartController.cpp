#include "CartController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../exception/ApiException.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::unique_ptr<service::CartService> CartController::GetCartService() {
    sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
    auto cart_repo = repository::RepositoryFactory::CreateCartRepository(db);
    auto prod_repo = repository::RepositoryFactory::CreateProductRepository(db);
    return std::make_unique<service::CartService>(cart_repo, prod_repo);
}

void CartController::GetCart(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Please login to access your shopping cart");
        }

        auto svc = GetCartService();
        auto cart = svc->GetUserCart(user_id, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(cart.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void CartController::AddItem(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Please login to add items to cart");
        }

        auto json_val = req->getJsonObject();
        if (!json_val) {
            throw exception::ValidationException("Request body must be valid JSON");
        }

        dto::AddToCartRequestDto dto;
        dto.product_id = json_val->get("product_id", 0).asInt64();
        dto.quantity = json_val->get("quantity", 1).asInt();

        auto svc = GetCartService();
        auto cart = svc->AddItem(user_id, dto, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(cart.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void CartController::UpdateItem(const drogon::HttpRequestPtr& req,
                                std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                int64_t id) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Authentication required");
        }

        auto json_val = req->getJsonObject();
        if (!json_val) {
            throw exception::ValidationException("Request body must be valid JSON");
        }

        int32_t qty = json_val->get("quantity", 1).asInt();

        auto svc = GetCartService();
        auto cart = svc->UpdateItemQuantity(user_id, id, qty, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(cart.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void CartController::RemoveItem(const drogon::HttpRequestPtr& req,
                                std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                int64_t id) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Authentication required");
        }

        auto svc = GetCartService();
        auto cart = svc->RemoveItem(user_id, id, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(cart.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void CartController::ClearCart(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Authentication required");
        }

        auto svc = GetCartService();
        svc->ClearUserCart(user_id, req_id);

        nlohmann::json data = {{"message", "Cart cleared successfully"}};
        callback(CreateJsonResponse(dto::ApiResponse::Success(data), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
