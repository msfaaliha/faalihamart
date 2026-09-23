#pragma once

#include "IPaymentStrategy.h"

namespace faaliha::faalihamart::service::payment {

/**
 * @brief Mock payment strategy for test and offline checkout simulation.
 */
class MockPaymentStrategy : public IPaymentStrategy {
public:
    PaymentResult ProcessPayment(const model::Money& amount, const std::string& payment_method) override;
};

} // namespace faaliha::faalihamart::service::payment
