#pragma once

#if __has_include(<drogon/HttpFilter.h>)
#include <drogon/HttpFilter.h>
#define HAS_DROGON 1
#else
#define HAS_DROGON 0
#endif

namespace faaliha::faalihamart::filter {

#if HAS_DROGON
class CORSFilter : public drogon::HttpFilter<CORSFilter> {
public:
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& fcb,
                  drogon::FilterChainCallback&& fccb) override;
};
#endif

} // namespace faaliha::faalihamart::filter
