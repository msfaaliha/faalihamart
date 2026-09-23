#pragma once

#include "BaseController.h"
#include "../service/OrderService.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class OrderController : public drogon::HttpController<OrderController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(OrderController::Checkout, "/api/v1/orders/checkout", drogon::Post);
    ADD_METHOD_TO(OrderController::GetBuyerOrders, "/api/v1/orders/buyer", drogon::Get);
    ADD_METHOD_TO(OrderController::GetSellerOrders, "/api/v1/orders/seller", drogon::Get);
    ADD_METHOD_TO(OrderController::GetOrder, "/api/v1/orders/{id}", drogon::Get);
    ADD_METHOD_TO(OrderController::UpdateStatus, "/api/v1/orders/{id}/status", drogon::Put);
    METHOD_LIST_END

    void Checkout(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetBuyerOrders(const drogon::HttpRequestPtr& req,
                        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetSellerOrders(const drogon::HttpRequestPtr& req,
                         std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetOrder(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                  int64_t id);

    void UpdateStatus(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      int64_t id);

private:
    std::unique_ptr<service::OrderService> GetOrderService();
};

} // namespace faaliha::faalihamart::controller
#endif
