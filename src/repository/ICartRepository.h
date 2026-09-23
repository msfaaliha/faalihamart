#pragma once

#include <vector>
#include <optional>
#include <memory>
#include "../model/CartItem.h"

namespace faaliha::faalihamart::repository {

/**
 * @brief Repository interface for shopping cart data access.
 */
class ICartRepository {
public:
    virtual ~ICartRepository() = default;

    /**
     * @brief Get all items in a user's shopping cart with joined product info.
     * @param user_id The buyer's ID.
     * @return Vector of cart items.
     */
    virtual std::vector<model::CartItem> FindByUserId(int64_t user_id) = 0;

    /**
     * @brief Find a specific cart item by ID.
     * @param id The cart item primary key.
     * @return Optional containing the cart item.
     */
    virtual std::optional<model::CartItem> FindById(int64_t id) = 0;

    /**
     * @brief Find cart item by user ID and product ID.
     * @param user_id User identifier.
     * @param product_id Product identifier.
     * @return Optional containing the cart item.
     */
    virtual std::optional<model::CartItem> FindByUserAndProduct(int64_t user_id, int64_t product_id) = 0;

    /**
     * @brief Add a product to cart or increment quantity if already present.
     * @param item CartItem entity.
     * @return Persisted cart item.
     */
    virtual model::CartItem Save(const model::CartItem& item) = 0;

    /**
     * @brief Remove an item from the cart.
     * @param id Cart item ID.
     * @param user_id Owner's user ID.
     * @return true if deleted.
     */
    virtual bool Delete(int64_t id, int64_t user_id) = 0;

    /**
     * @brief Clear all items from a user's cart (called upon checkout).
     * @param user_id The buyer's ID.
     * @return true if cleared.
     */
    virtual bool ClearCart(int64_t user_id) = 0;
};

} // namespace faaliha::faalihamart::repository
