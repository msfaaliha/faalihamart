#pragma once

#include <string>
#include <vector>

#if __has_include(<drogon/HttpFilter.h>)
#include <drogon/HttpFilter.h>
#define HAS_DROGON 1
#else
#define HAS_DROGON 0
#endif

namespace faaliha::faalihamart::filter {

#if HAS_DROGON
/**
 * @brief Base authentication and session check filter.
 */
class AuthFilter : public drogon::HttpFilter<AuthFilter> {
public:
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& fcb,
                  drogon::FilterChainCallback&& fccb) override;
};

/**
 * @brief Seller role authorization filter.
 */
class SellerAuthFilter : public drogon::HttpFilter<SellerAuthFilter> {
public:
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& fcb,
                  drogon::FilterChainCallback&& fccb) override;
};

/**
 * @brief Admin role authorization filter.
 */
class AdminAuthFilter : public drogon::HttpFilter<AdminAuthFilter> {
public:
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& fcb,
                  drogon::FilterChainCallback&& fccb) override;
};
#endif

} // namespace faaliha::faalihamart::filter
