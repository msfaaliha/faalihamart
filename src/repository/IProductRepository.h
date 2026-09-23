#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "../model/Product.h"
#include "../dto/ProductDto.h"

namespace faaliha::faalihamart::repository {

/**
 * @brief Repository interface for Product entity data access abstraction.
 * All queries must be strictly parameterized.
 */
class IProductRepository {
public:
    virtual ~IProductRepository() = default;

    /**
     * @brief Find a single product by primary key.
     * @param id The product identifier.
     * @return Optional containing the product if found.
     */
    virtual std::optional<model::Product> FindById(int64_t id) = 0;

    /**
     * @brief Query products matching category, keyword, and price filters.
     * @param filter The filter parameters.
     * @return Vector of matching products.
     */
    virtual std::vector<model::Product> FindAll(const dto::ProductFilterDto& filter) = 0;

    /**
     * @brief Find all products listed by a specific seller.
     * @param seller_id The seller's user ID.
     * @return Vector of products belonging to the seller.
     */
    virtual std::vector<model::Product> FindBySellerId(int64_t seller_id) = 0;

    /**
     * @brief Create a new product listing.
     * @param product Product to insert.
     * @return The persisted product with generated ID.
     */
    virtual model::Product Create(const model::Product& product) = 0;

    /**
     * @brief Update an existing product listing.
     * @param product Updated product details.
     * @return Updated product.
     */
    virtual model::Product Update(const model::Product& product) = 0;

    /**
     * @brief Delete a product listing by ID.
     * @param id The product ID.
     * @return true if deleted, false if not found.
     */
    virtual bool Delete(int64_t id) = 0;

    /**
     * @brief Update stock quantity atomically.
     * @param id Product ID.
     * @param delta Stock difference (+ or -).
     * @return true if stock was successfully updated.
     */
    virtual bool UpdateStock(int64_t id, int32_t delta) = 0;

    /**
     * @brief Count total products in the catalog.
     * @return Count of products.
     */
    virtual int64_t Count() = 0;
};

} // namespace faaliha::faalihamart::repository
