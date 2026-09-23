#include "ReviewController.h"
#include "../plugin/DbPlugin.h"
#include "../repository/RepositoryFactory.h"
#include "../exception/ApiException.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

std::unique_ptr<service::ReviewService> ReviewController::GetReviewService() {
    sqlite3* db = plugin::DbPlugin::GetInstance().GetDb();
    auto review_repo = repository::RepositoryFactory::CreateReviewRepository(db);
    auto order_repo = repository::RepositoryFactory::CreateOrderRepository(db);
    return std::make_unique<service::ReviewService>(review_repo, order_repo);
}

void ReviewController::GetProductReviews(const drogon::HttpRequestPtr& req,
                                         std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                         int64_t id) {
    std::string req_id = GetRequestId(req);
    try {
        auto svc = GetReviewService();
        auto reviews = svc->GetProductReviews(id, req_id);

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& r : reviews) {
            arr.push_back(r.ToJson());
        }

        callback(CreateJsonResponse(dto::ApiResponse::Success(arr), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void ReviewController::AddReview(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string req_id = GetRequestId(req);
    try {
        int64_t user_id = GetCurrentUserId(req);
        if (user_id <= 0) {
            throw exception::UnauthorizedException("Please login to post a review");
        }

        auto json_val = req->getJsonObject();
        if (!json_val) {
            throw exception::ValidationException("Request body must be valid JSON");
        }

        dto::CreateReviewRequestDto dto;
        dto.product_id = json_val->get("product_id", 0).asInt64();
        dto.rating = json_val->get("rating", 5).asInt();
        dto.comment = json_val->get("comment", "").asString();

        auto svc = GetReviewService();
        auto review = svc->AddReview(user_id, dto, req_id);

        callback(CreateJsonResponse(dto::ApiResponse::Success(review.ToJson()), drogon::k201Created));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

void ReviewController::DeleteReview(const drogon::HttpRequestPtr& req,
                                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                    int64_t id) {
    std::string req_id = GetRequestId(req);
    try {
        std::string role = GetCurrentUserRole(req);
        bool is_admin = (role == "ADMIN");
        if (!is_admin) {
            throw exception::ForbiddenException("Admin access required to delete reviews");
        }

        auto svc = GetReviewService();
        svc->DeleteReview(id, is_admin, req_id);

        nlohmann::json data = {{"message", "Review deleted successfully"}};
        callback(CreateJsonResponse(dto::ApiResponse::Success(data), drogon::k200OK));
    } catch (const std::exception& ex) {
        callback(HandleError(ex, req_id));
    }
}

} // namespace faaliha::faalihamart::controller
#endif
