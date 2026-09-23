#include <gtest/gtest.h>
#include "model/Money.h"

using namespace faaliha::faalihamart::model;

TEST(MoneyTest, ConstructsFromCents) {
    Money m = Money::FromCents(14999);
    EXPECT_EQ(m.GetCents(), 14999);
    EXPECT_DOUBLE_EQ(m.ToRupees(), 149.99);
    EXPECT_DOUBLE_EQ(m.ToDollars(), 149.99);
    EXPECT_EQ(m.ToString(), "₹149.99");
}

TEST(MoneyTest, ConstructsFromRupees) {
    Money m = Money::FromRupees(49.99);
    EXPECT_EQ(m.GetCents(), 4999);
    EXPECT_DOUBLE_EQ(m.ToRupees(), 49.99);
    EXPECT_EQ(m.ToString(), "₹49.99");
}

TEST(MoneyTest, FormatsSmallValues) {
    Money m = Money::FromCents(5);
    EXPECT_EQ(m.ToString(), "₹0.05");

    Money zero = Money::FromCents(0);
    EXPECT_EQ(zero.ToString(), "₹0.00");
}

TEST(MoneyTest, ArithmeticAdditionAndSubtraction) {
    Money a = Money::FromCents(1000); // ₹10.00
    Money b = Money::FromCents(450);  // ₹4.50

    Money sum = a + b;
    EXPECT_EQ(sum.GetCents(), 1450);
    EXPECT_EQ(sum.ToString(), "₹14.50");

    Money diff = a - b;
    EXPECT_EQ(diff.GetCents(), 550);
    EXPECT_EQ(diff.ToString(), "₹5.50");
}

TEST(MoneyTest, NegativeBalanceThrowsException) {
    Money small = Money::FromCents(100);
    Money large = Money::FromCents(500);
    EXPECT_THROW(small - large, std::invalid_argument);
}

TEST(MoneyTest, MultiplicationAndDivision) {
    Money unit_price = Money::FromCents(2500); // ₹25.00
    Money total = unit_price * 4;
    EXPECT_EQ(total.GetCents(), 10000);
    EXPECT_EQ(total.ToString(), "₹100.00");

    Money divided = total / 2;
    EXPECT_EQ(divided.GetCents(), 5000);
    EXPECT_THROW(total / 0, std::invalid_argument);
}
