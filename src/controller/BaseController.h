#pragma once

#include <string>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>
#include "../dto/ApiResponse.h"
#include "../exception/GlobalExceptionHandler.h"

#if __has_include(<drogon/HttpController.h>)
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#define HAS_DROGON_CTRL 1
#else
#define HAS_DROGON_CTRL 0
#endif

namespace faaliha::faalihamart::controller {

/**
 * @brief Common base controller providing Front Controller pattern facilities,
 * standard error envelopes, and request context extraction.
 */
class BaseController {
public:
    virtual ~BaseController() = default;

#if HAS_DROGON_CTRL
    static std::string GetRequestId(const drogon::HttpRequestPtr& req) {
        if (req->attributes()->find("request_id")) {
            return req->attributes()->get<std::string>("request_id");
        }
        return "req-direct";
    }

    static int64_t GetCurrentUserId(const drogon::HttpRequestPtr& req) {
        auto session = req->session();
        if (session && session->find("user_id")) {
            return session->get<int64_t>("user_id");
        }
        return 0;
    }

    static std::string GetCurrentUserRole(const drogon::HttpRequestPtr& req) {
        auto session = req->session();
        if (session && session->find("role")) {
            return session->get<std::string>("role");
        }
        return "BUYER";
    }

    static drogon::HttpResponsePtr CreateJsonResponse(const nlohmann::json& j, drogon::HttpStatusCode code = drogon::k200OK) {
        std::string s = j.dump();
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(code);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        resp->setBody(std::move(s));
        return resp;
    }

    static drogon::HttpResponsePtr HandleError(const std::exception& ex, const std::string& request_id) {
        auto formatted = exception::GlobalExceptionHandler::HandleException(ex, request_id);
        return CreateJsonResponse(formatted.response_json, static_cast<drogon::HttpStatusCode>(formatted.http_status));
    }
#endif
};

} // namespace faaliha::faalihamart::controller
