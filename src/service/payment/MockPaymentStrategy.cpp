#include "MockPaymentStrategy.h"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace faaliha::faalihamart::service::payment {

PaymentResult MockPaymentStrategy::ProcessPayment(const model::Money& amount, const std::string& payment_method) {
    if (amount.GetCents() <= 0) {
        return PaymentResult{false, "", "Payment amount must be greater than zero"};
    }

    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::ostringstream tx_ss;
    tx_ss << "TX-MOCK-" << ms;

    return PaymentResult{
        true,
        tx_ss.str(),
        "Payment authorized successfully via " + (payment_method.empty() ? "MOCK_PAYMENT" : payment_method)
    };
}

} // namespace faaliha::faalihamart::service::payment
