#pragma once

#include "BaseController.h"
#include "../service/ReviewService.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class ReviewController : public drogon::HttpController<ReviewController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ReviewController::GetProductReviews, "/api/v1/products/{id}/reviews", drogon::Get);
    ADD_METHOD_TO(ReviewController::AddReview, "/api/v1/reviews", drogon::Post);
    ADD_METHOD_TO(ReviewController::DeleteReview, "/api/v1/reviews/{id}", drogon::Delete);
    METHOD_LIST_END

    void GetProductReviews(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                           int64_t id);

    void AddReview(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void DeleteReview(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      int64_t id);

private:
    std::unique_ptr<service::ReviewService> GetReviewService();
};

} // namespace faaliha::faalihamart::controller
#endif
