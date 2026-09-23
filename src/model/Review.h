#pragma once

#include <cstdint>
#include <string>

namespace faaliha::faalihamart::model {

/**
 * @brief Review domain model.
 */
struct Review {
    int64_t id_{0};
    int64_t product_id_{0};
    int64_t user_id_{0};
    int32_t rating_{5};
    std::string comment_;
    std::string created_at_;

    // Joined reviewer name
    std::string user_name_;
};

} // namespace faaliha::faalihamart::model
