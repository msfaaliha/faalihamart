#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace faaliha::faalihamart::dto {

/**
 * @brief Fixed standard JSON response envelope per Section 11.
 */
class ApiResponse {
public:
    static nlohmann::json Success(const nlohmann::json& data = nlohmann::json::object()) {
        nlohmann::json response;
        response["success"] = true;
        response["data"] = data;
        response["error"] = nullptr;
        return response;
    }

    static nlohmann::json Error(const std::string& code, const std::string& message, const std::vector<std::string>& details = {}) {
        nlohmann::json response;
        response["success"] = false;
        response["data"] = nullptr;
        nlohmann::json errorObj;
        errorObj["code"] = code;
        errorObj["message"] = message;
        if (!details.empty()) {
            errorObj["details"] = details;
        }
        response["error"] = errorObj;
        return response;
    }
};

} // namespace faaliha::faalihamart::dto
