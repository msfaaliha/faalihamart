#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <sstream>

namespace faaliha::faalihamart::model {

/**
 * @brief Represents monetary value stored in integer minor units (cents).
 * Ensures no floating-point inaccuracies or raw integer arithmetic leaks into business logic.
 */
class Money {
public:
    constexpr Money() noexcept : cents_(0) {}
    constexpr explicit Money(int64_t cents) noexcept : cents_(cents) {}

    static constexpr Money FromCents(int64_t cents) noexcept {
        return Money(cents);
    }

    static Money FromDollars(double dollars) {
        if (dollars < 0) {
            throw std::invalid_argument("Monetary amount cannot be negative");
        }
        return Money(static_cast<int64_t>(dollars * 100.0 + 0.5));
    }

    static Money FromRupees(double rupees) {
        if (rupees < 0) {
            throw std::invalid_argument("Monetary amount cannot be negative");
        }
        return Money(static_cast<int64_t>(rupees * 100.0 + 0.5));
    }

    [[nodiscard]] constexpr int64_t GetCents() const noexcept {
        return cents_;
    }

    [[nodiscard]] double ToRupees() const noexcept {
        return static_cast<double>(cents_) / 100.0;
    }

    [[nodiscard]] double ToDollars() const noexcept {
        return ToRupees();
    }

    [[nodiscard]] std::string ToString() const {
        std::ostringstream oss;
        int64_t abs_cents = cents_ >= 0 ? cents_ : -cents_;
        int64_t rupees = abs_cents / 100;
        int64_t remainder = abs_cents % 100;

        if (cents_ < 0) {
            oss << "-";
        }
        oss << "₹" << rupees << "." << std::setfill('0') << std::setw(2) << remainder;
        return oss.str();
    }

    constexpr Money operator+(const Money& other) const noexcept {
        return Money(cents_ + other.cents_);
    }

    Money operator-(const Money& other) const {
        if (cents_ < other.cents_) {
            throw std::invalid_argument("Money subtraction resulted in negative balance");
        }
        return Money(cents_ - other.cents_);
    }

    constexpr Money operator*(int64_t factor) const noexcept {
        return Money(cents_ * factor);
    }

    Money operator/(int64_t divisor) const {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero in Money calculation");
        }
        return Money(cents_ / divisor);
    }

    constexpr auto operator<=>(const Money& other) const noexcept = default;
    constexpr bool operator==(const Money& other) const noexcept = default;

private:
    int64_t cents_{0};
};

} // namespace faaliha::faalihamart::model
