#include "CORSFilter.h"

namespace faaliha::faalihamart::filter {

#if HAS_DROGON
void CORSFilter::doFilter(const drogon::HttpRequestPtr& req,
                          drogon::FilterCallback&& fcb,
                          drogon::FilterChainCallback&& fccb) {
    if (req->method() == drogon::HttpMethod::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        resp->addHeader("Access-Control-Allow-Credentials", "true");
        resp->setStatusCode(drogon::k204NoContent);
        fcb(resp);
        return;
    }
    fccb();
}
#endif

} // namespace faaliha::faalihamart::filter
