#pragma once

#include <string>
#include <memory>
#include "../../model/Money.h"

namespace faaliha::faalihamart::service::payment {

struct PaymentResult {
    bool success{false};
    std::string transaction_id;
    std::string message;
};

/**
 * @brief Strategy interface for swappable payment processing (Section 10).
 */
class IPaymentStrategy {
public:
    virtual ~IPaymentStrategy() = default;

    /**
     * @brief Process a payment transaction.
     * @param amount The monetary amount.
     * @param payment_method Payment token or method.
     * @return PaymentResult indicating outcome.
     */
    virtual PaymentResult ProcessPayment(const model::Money& amount, const std::string& payment_method) = 0;
};

} // namespace faaliha::faalihamart::service::payment
