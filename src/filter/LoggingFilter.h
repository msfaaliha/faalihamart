#pragma once

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

#if __has_include(<drogon/HttpFilter.h>)
#include <drogon/HttpFilter.h>
#define HAS_DROGON 1
#else
#define HAS_DROGON 0
#endif

namespace faaliha::faalihamart::filter {

inline std::string GenerateRequestId() {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    uint64_t val = rng();
    std::ostringstream ss;
    ss << "req-" << std::hex << std::setw(16) << std::setfill('0') << val;
    return ss.str();
}

#if HAS_DROGON
class LoggingFilter : public drogon::HttpFilter<LoggingFilter> {
public:
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& fcb,
                  drogon::FilterChainCallback&& fccb) override;
};
#endif

} // namespace faaliha::faalihamart::filter
