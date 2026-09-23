#pragma once

#include "BaseController.h"
#include "../service/CartService.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class CartController : public drogon::HttpController<CartController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CartController::GetCart, "/api/v1/cart", drogon::Get);
    ADD_METHOD_TO(CartController::AddItem, "/api/v1/cart/items", drogon::Post);
    ADD_METHOD_TO(CartController::UpdateItem, "/api/v1/cart/items/{id}", drogon::Put);
    ADD_METHOD_TO(CartController::RemoveItem, "/api/v1/cart/items/{id}", drogon::Delete);
    ADD_METHOD_TO(CartController::ClearCart, "/api/v1/cart", drogon::Delete);
    METHOD_LIST_END

    void GetCart(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void AddItem(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void UpdateItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    int64_t id);

    void RemoveItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    int64_t id);

    void ClearCart(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::unique_ptr<service::CartService> GetCartService();
};

} // namespace faaliha::faalihamart::controller
#endif
