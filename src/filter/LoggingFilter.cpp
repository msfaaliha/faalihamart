#include "LoggingFilter.h"
#include <spdlog/spdlog.h>

namespace faaliha::faalihamart::filter {

#if HAS_DROGON
void LoggingFilter::doFilter(const drogon::HttpRequestPtr& req,
                             drogon::FilterCallback&& /*fcb*/,
                             drogon::FilterChainCallback&& fccb) {
    std::string req_id = GenerateRequestId();
    req->attributes()->insert("request_id", req_id);

    std::string path = req->path();
    std::string method = req->methodString();
    spdlog::info("[{}] --> HTTP {} {}", req_id, method, path);

    fccb();
}
#endif

} // namespace faaliha::faalihamart::filter
