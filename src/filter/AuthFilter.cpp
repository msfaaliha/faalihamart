#include "AuthFilter.h"
#include "../dto/ApiResponse.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::filter {

#if HAS_DROGON
void AuthFilter::doFilter(const drogon::HttpRequestPtr& req,
                          drogon::FilterCallback&& fcb,
                          drogon::FilterChainCallback&& fccb) {
    auto session = req->session();
    if (!session || !session->find("user_id")) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::ApiResponse::Error("UNAUTHORIZED", "Session expired or authentication required")
        );
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }
    fccb();
}

void SellerAuthFilter::doFilter(const drogon::HttpRequestPtr& req,
                                drogon::FilterCallback&& fcb,
                                drogon::FilterChainCallback&& fccb) {
    auto session = req->session();
    if (!session || !session->find("user_id")) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::ApiResponse::Error("UNAUTHORIZED", "Authentication required")
        );
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    std::string role = session->get<std::string>("role");
    if (role != "SELLER" && role != "ADMIN") {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::ApiResponse::Error("FORBIDDEN", "Seller privileges required")
        );
        resp->setStatusCode(drogon::k403Forbidden);
        fcb(resp);
        return;
    }
    fccb();
}

void AdminAuthFilter::doFilter(const drogon::HttpRequestPtr& req,
                               drogon::FilterCallback&& fcb,
                               drogon::FilterChainCallback&& fccb) {
    auto session = req->session();
    if (!session || !session->find("user_id")) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::ApiResponse::Error("UNAUTHORIZED", "Authentication required")
        );
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    std::string role = session->get<std::string>("role");
    if (role != "ADMIN") {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::ApiResponse::Error("FORBIDDEN", "Admin privileges required")
        );
        resp->setStatusCode(drogon::k403Forbidden);
        fcb(resp);
        return;
    }
    fccb();
}
#endif

} // namespace faaliha::faalihamart::filter
