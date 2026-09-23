#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../repository/IProductRepository.h"
#include "../dto/ProductDto.h"

namespace faaliha::faalihamart::service {

/**
 * @brief Manages product catalog, search/filtering, and seller product operations.
 */
class ProductService {
public:
    explicit ProductService(std::shared_ptr<repository::IProductRepository> product_repo);

    /**
     * @brief Get product details by ID.
     * @param id Product ID.
     * @param request_id Request tracking ID.
     * @return ProductResponseDto.
     * @throws NotFoundException if product doesn't exist.
     */
    dto::ProductResponseDto GetProductById(int64_t id, const std::string& request_id = "");

    /**
     * @brief Search and filter products for buyer browsing.
     * @param filter Filter criteria.
     * @param request_id Request tracking ID.
     * @return Vector of ProductResponseDto.
     */
    std::vector<dto::ProductResponseDto> SearchProducts(const dto::ProductFilterDto& filter, const std::string& request_id = "");

    /**
     * @brief Get all products belonging to a seller.
     * @param seller_id Seller user ID.
     * @param request_id Request tracking ID.
     * @return Vector of ProductResponseDto.
     */
    std::vector<dto::ProductResponseDto> GetSellerProducts(int64_t seller_id, const std::string& request_id = "");

    /**
     * @brief Create a new product listing (Seller operation).
     * @param seller_id The authenticated seller's ID.
     * @param req Product creation details.
     * @param request_id Request tracking ID.
     * @return Created ProductResponseDto.
     */
    dto::ProductResponseDto CreateProduct(int64_t seller_id, const dto::ProductRequestDto& req, const std::string& request_id = "");

    /**
     * @brief Update an existing product listing (Seller operation).
     * @param product_id Product ID to update.
     * @param seller_id The authenticated seller's ID.
     * @param req Updated product details.
     * @param request_id Request tracking ID.
     * @return Updated ProductResponseDto.
     */
    dto::ProductResponseDto UpdateProduct(int64_t product_id, int64_t seller_id, const dto::ProductRequestDto& req, const std::string& request_id = "");

    /**
     * @brief Delete a product listing (Seller ownership or Admin moderation).
     * @param product_id Product ID.
     * @param requesting_user_id ID of seller or admin.
     * @param is_admin Whether user has ADMIN role.
     * @param request_id Request tracking ID.
     * @return true if deleted.
     */
    bool DeleteProduct(int64_t product_id, int64_t requesting_user_id, bool is_admin, const std::string& request_id = "");

private:
    std::shared_ptr<repository::IProductRepository> product_repo_;
};

} // namespace faaliha::faalihamart::service
