#pragma once

#include <string>
#include <nlohmann/json.hpp>

#if __has_include(<json/json.h>)
#include <json/json.h>
#define HAS_JSONCPP 1
#else
#define HAS_JSONCPP 0
#endif

namespace faaliha::faalihamart::util {

class JsonUtil {
public:
    static std::string DumpToString(const nlohmann::json& j) {
        return j.dump();
    }

    static nlohmann::json ParseFromString(const std::string& str) {
        return nlohmann::json::parse(str);
    }

#if HAS_JSONCPP
    static Json::Value ToJsonCpp(const nlohmann::json& nj) {
        std::string s = nj.dump();
        Json::CharReaderBuilder builder;
        Json::Value root;
        std::string errs;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        reader->parse(s.data(), s.data() + s.size(), &root, &errs);
        return root;
    }

    static nlohmann::json FromJsonCpp(const Json::Value& val) {
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::string s = Json::writeString(builder, val);
        return nlohmann::json::parse(s);
    }
#endif
};

} // namespace faaliha::faalihamart::util
