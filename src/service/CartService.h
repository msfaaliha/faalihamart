#pragma once

#include <memory>
#include <string>
#include "../repository/ICartRepository.h"
#include "../repository/IProductRepository.h"
#include "../dto/CartDto.h"

namespace faaliha::faalihamart::service {

/**
 * @brief Manages shopping cart operations and running totals.
 */
class CartService {
public:
    CartService(std::shared_ptr<repository::ICartRepository> cart_repo,
                std::shared_ptr<repository::IProductRepository> product_repo);

    /**
     * @brief Retrieve all cart items with calculated running total for a user.
     * @param user_id The buyer's ID.
     * @param request_id Request tracking ID.
     * @return CartResponseDto with items and running total.
     */
    dto::CartResponseDto GetUserCart(int64_t user_id, const std::string& request_id = "");

    /**
     * @brief Add an item to the shopping cart.
     * @param user_id The buyer's ID.
     * @param req Add to cart request.
     * @param request_id Request tracking ID.
     * @return Updated CartResponseDto.
     */
    dto::CartResponseDto AddItem(int64_t user_id, const dto::AddToCartRequestDto& req, const std::string& request_id = "");

    /**
     * @brief Update quantity of an item in the cart.
     * @param user_id The buyer's ID.
     * @param cart_item_id The cart item primary key.
     * @param quantity New quantity (if <= 0, item is removed).
     * @param request_id Request tracking ID.
     * @return Updated CartResponseDto.
     */
    dto::CartResponseDto UpdateItemQuantity(int64_t user_id, int64_t cart_item_id, int32_t quantity, const std::string& request_id = "");

    /**
     * @brief Remove an item from the cart.
     * @param user_id The buyer's ID.
     * @param cart_item_id Cart item ID.
     * @param request_id Request tracking ID.
     * @return Updated CartResponseDto.
     */
    dto::CartResponseDto RemoveItem(int64_t user_id, int64_t cart_item_id, const std::string& request_id = "");

    /**
     * @brief Clear all items from cart.
     * @param user_id Buyer's ID.
     * @param request_id Request tracking ID.
     */
    void ClearUserCart(int64_t user_id, const std::string& request_id = "");

private:
    std::shared_ptr<repository::ICartRepository> cart_repo_;
    std::shared_ptr<repository::IProductRepository> product_repo_;
};

} // namespace faaliha::faalihamart::service
