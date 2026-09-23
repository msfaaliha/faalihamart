#include <gtest/gtest.h>
#include "util/ValidationUtil.h"
#include "exception/ApiException.h"

using namespace faaliha::faalihamart::util;
using namespace faaliha::faalihamart::exception;

TEST(ValidationUtilTest, ValidEmailPasses) {
    EXPECT_NO_THROW(ValidationUtil::ValidateEmail("user@faalihamart.com"));
    EXPECT_NO_THROW(ValidationUtil::ValidateEmail("alice.smith+test@domain.co.uk"));
}

TEST(ValidationUtilTest, InvalidEmailThrows) {
    EXPECT_THROW(ValidationUtil::ValidateEmail(""), ValidationException);
    EXPECT_THROW(ValidationUtil::ValidateEmail("plainaddress"), ValidationException);
    EXPECT_THROW(ValidationUtil::ValidateEmail("@missingusername.com"), ValidationException);
    EXPECT_THROW(ValidationUtil::ValidateEmail("user@domain"), ValidationException);
}

TEST(ValidationUtilTest, PasswordStrength) {
    EXPECT_NO_THROW(ValidationUtil::ValidatePassword("StrongPass123!"));
    EXPECT_THROW(ValidationUtil::ValidatePassword("short1!"), ValidationException); // < 8
    EXPECT_THROW(ValidationUtil::ValidatePassword("allletterslowercase"), ValidationException); // no digits
    EXPECT_THROW(ValidationUtil::ValidatePassword("1234567890"), ValidationException); // no letters
}

TEST(ValidationUtilTest, RatingRangeValidation) {
    EXPECT_NO_THROW(ValidationUtil::ValidateRating(1));
    EXPECT_NO_THROW(ValidationUtil::ValidateRating(3));
    EXPECT_NO_THROW(ValidationUtil::ValidateRating(5));

    EXPECT_THROW(ValidationUtil::ValidateRating(0), ValidationException);
    EXPECT_THROW(ValidationUtil::ValidateRating(6), ValidationException);
}

TEST(ValidationUtilTest, RoleValidation) {
    EXPECT_NO_THROW(ValidationUtil::ValidateRole("BUYER"));
    EXPECT_NO_THROW(ValidationUtil::ValidateRole("SELLER"));
    EXPECT_THROW(ValidationUtil::ValidateRole("ADMIN"), ValidationException); // Admin not allowed via public signup
    EXPECT_THROW(ValidationUtil::ValidateRole("SUPERUSER"), ValidationException);
}
