#pragma once

#include <sqlite3.h>
#include <memory>
#include <mutex>
#include <string>

#if __has_include(<drogon/plugins/Plugin.h>)
#include <drogon/plugins/Plugin.h>
#define HAS_DROGON_PLUGIN 1
#else
#define HAS_DROGON_PLUGIN 0
#endif

namespace faaliha::faalihamart::plugin {

/**
 * @brief Singleton Database Plugin owning database connection lifecycle and migration startup.
 */
#if HAS_DROGON_PLUGIN
class DbPlugin : public drogon::Plugin<DbPlugin> {
public:
    DbPlugin() = default;
    void initAndStart(const Json::Value& config) override;
    void shutdown() override;

    static DbPlugin& GetInstance();
    sqlite3* GetDb();

private:
    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
    static DbPlugin* instance_;
};
#else
class DbPlugin {
public:
    static DbPlugin& GetInstance();
    void Init(const std::string& db_path = "faalihamart.db");
    void Shutdown();
    sqlite3* GetDb();

private:
    sqlite3* db_{nullptr};
    std::mutex db_mutex_;
};
#endif

} // namespace faaliha::faalihamart::plugin
