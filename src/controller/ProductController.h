#pragma once

#include "BaseController.h"
#include "../service/ProductService.h"

#if HAS_DROGON_CTRL
namespace faaliha::faalihamart::controller {

class ProductController : public drogon::HttpController<ProductController>, public BaseController {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ProductController::ListProducts, "/api/v1/products", drogon::Get);
    ADD_METHOD_TO(ProductController::GetProduct, "/api/v1/products/{id}", drogon::Get);
    ADD_METHOD_TO(ProductController::CreateProduct, "/api/v1/products", drogon::Post);
    ADD_METHOD_TO(ProductController::UpdateProduct, "/api/v1/products/{id}", drogon::Put);
    ADD_METHOD_TO(ProductController::DeleteProduct, "/api/v1/products/{id}", drogon::Delete);
    METHOD_LIST_END

    void ListProducts(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void GetProduct(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    int64_t id);

    void CreateProduct(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void UpdateProduct(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                       int64_t id);

    void DeleteProduct(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                       int64_t id);

private:
    std::unique_ptr<service::ProductService> GetProductService();
};

} // namespace faaliha::faalihamart::controller
#endif
