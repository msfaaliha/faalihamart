#include "ProductController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../exception/ApiException.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::unique_ptr<service::ProductService> ProductController::GetProductService() {
    sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
    auto repo = repository::RepositoryFactory::CreateProductRepository(db);
    return std::make_unique<service::ProductService>(repo);
}

void ProductController::ListProducts(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        dto::ProductFilterDto filter;
        std::string cat = req->getParameter("category");
        std::string kw = req->getParameter("keyword");
        std::string sid = req->getParameter("seller_id");

        if (!cat.empty()) filter.category = cat;
        if (!kw.empty()) filter.keyword = kw;
        if (!sid.empty()) filter.seller_id = std::stoll(sid);

        auto svc = GetProductService();
        auto products = svc->SearchProducts(filter, req_id);

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& p : products) {
            arr.push_back(p.ToJson());
        }

        callback(CreateJsonResponse(dto::ApiResponse::Success(arr), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void ProductController::GetProduct(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                   int64_t id) {
    std::string req_id = GetRequestId(req);
    try {
        auto svc = GetProductService();
        auto prod = svc->GetProductById(id, req_id);
        callback(CreateJsonResponse(dto::ApiResponse::Success(prod.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void ProductController::CreateProduct(const drogon::HttpRequestPtr& req,
                                      std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        std::string role = GetCurrentUserRole(req);
        if (user_id <= 0 || (role != "SELLER" && role != "ADMIN")) {
            throw exception::ForbiddenException("Seller account required to create listings");
        }

        auto json_val = req->getJsonObject();
        if (!json_val) {
            throw exception::ValidationException("Request body must be valid JSON");
        }

        dto::ProductRequestDto dto;
        dto.name = json_val->get("name", "").asString();
        dto.description = json_val->get("description", "").asString();
        dto.price_cents = json_val->get("price_cents", 0).asInt64();
        dto.stock_qty = json_val->get("stock_qty", 0).asInt();
        dto.category = json_val->get("category", "").asString();
        dto.image_url = json_val->get("image_url", "").asString();

        auto svc = GetProductService();
        auto created = svc->CreateProduct(user_id, dto, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(created.ToJson()), drogon::k201Created));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void ProductController::UpdateProduct(const drogon::HttpRequestPtr& req,
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

        dto::ProductRequestDto dto;
        dto.name = json_val->get("name", "").asString();
        dto.description = json_val->get("description", "").asString();
        dto.price_cents = json_val->get("price_cents", 0).asInt64();
        dto.stock_qty = json_val->get("stock_qty", 0).asInt();
        dto.category = json_val->get("category", "").asString();
        dto.image_url = json_val->get("image_url", "").asString();

        auto svc = GetProductService();
        auto updated = svc->UpdateProduct(id, user_id, dto, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(updated.ToJson()), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void ProductController::DeleteProduct(const drogon::HttpRequestPtr& req,
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
        auto svc = GetProductService();
        svc->DeleteProduct(id, user_id, is_admin, req_id);

        nlohmann::json data = {{"message", "Product deleted successfully"}};
        callback(CreateJsonResponse(dto::ApiResponse::Success(data), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
