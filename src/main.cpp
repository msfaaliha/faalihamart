#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if __has_include(<drogon/drogon.h>)
#include <drogon/drogon.h>
#define HAS_DROGON_APP 1
#else
#define HAS_DROGON_APP 0
#endif

#include "plugin/DbPlugin.h"

int main(int argc, char* argv[]) {
    // Setup structured console logging via spdlog
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    auto logger = std::make_shared<spdlog::logger>("faalihamart", console_sink);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);

    spdlog::info("=================================================");
    spdlog::info("   FaalihaMart Native C++ Marketplace Backend    ");
    spdlog::info("   Target Milestone: Final Review (Oct 10, 2026) ");
    spdlog::info("   Namespace: faaliha::faalihamart               ");
    spdlog::info("=================================================");

#if HAS_DROGON_APP
    try {
        std::string config_path = "config.json";
        if (argc > 1) {
            config_path = argv[1];
        }

        if (std::filesystem::exists(config_path)) {
            spdlog::info("Loading configuration from: {}", config_path);
            drogon::app().loadConfigFile(config_path);
        } else {
            spdlog::warn("Config file '{}' not found, using default configuration", config_path);
            drogon::app()
                .addListener("0.0.0.0", 8080)
                .setThreadNum(4)
                .setDocumentRoot("./static")
                .enableSession(3600); // 1 hour idle session timeout
        }

        // Ensure static directory is served
        drogon::app().setDocumentRoot("./static");

        // Allow PORT environment variable override (e.g. on Render or cloud container platforms)
        const char* port_env = std::getenv("PORT");
        if (port_env) {
            try {
                int p = std::stoi(port_env);
                if (p > 0 && p <= 65535 && p != 8080) {
                    spdlog::info("PORT environment variable detected. Adding listener on 0.0.0.0:{}", p);
                    drogon::app().addListener("0.0.0.0", static_cast<uint16_t>(p));
                }
            } catch (...) {}
        }

        spdlog::info("Starting FaalihaMart Drogon HTTP server...");
        drogon::app().run();
    } catch (const std::exception& e) {
        spdlog::critical("Server terminated with unhandled exception: {}", e.what());
        return 1;
    }
#else
    spdlog::info("Drogon headers not in active path. Running standalone DB verification mode.");
    try {
        faaliha::faalihamart::plugin::DbPlugin::GetInstance().Init("faalihamart.db");
        spdlog::info("Database initialized and migrations applied successfully.");
    } catch (const std::exception& e) {
        spdlog::error("Standalone initialization error: {}", e.what());
        return 1;
    }
#endif

    return 0;
}
