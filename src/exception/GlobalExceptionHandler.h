#pragma once

#include <string>
#include <exception>
#include <nlohmann/json.hpp>
#include "ApiException.h"

namespace faaliha::faalihamart::exception {

struct FormattedError {
    int http_status{500};
    nlohmann::json response_json;
};

class GlobalExceptionHandler {
public:
    static FormattedError HandleException(const std::exception& ex, const std::string& request_id = "");
    static FormattedError HandleUnknownException(const std::string& request_id = "");
};

} // namespace faaliha::faalihamart::exception
