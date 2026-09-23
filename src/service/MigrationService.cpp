#include "MigrationService.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace faaliha::faalihamart::service {

MigrationService::MigrationService(std::shared_ptr<repository::IMigrationRepository> migration_repo)
    : migration_repo_(std::move(migration_repo)) {}

int MigrationService::RunMigrations(const std::string& migrations_dir) {
    spdlog::info("MigrationService: checking migrations in directory '{}'", migrations_dir);

    // 1. Ensure tracking table exists
    migration_repo_->EnsureMigrationTable();

    // 2. Fetch already applied versions
    auto applied = migration_repo_->GetAppliedVersions();

    if (!fs::exists(migrations_dir)) {
        spdlog::warn("MigrationService: migrations directory '{}' does not exist", migrations_dir);
        return 0;
    }

    // 3. Scan directory for V<n>__*.sql files
    struct MigrationFile {
        int version;
        std::string filename;
        std::string full_path;
    };

    std::vector<MigrationFile> files;
    const std::regex v_regex(R"(^V(\d+)__(.+)\.sql$)");

    for (const auto& entry : fs::directory_iterator(migrations_dir)) {
        if (!entry.is_regular_file()) continue;
        std::string fname = entry.path().filename().string();
        std::smatch match;
        if (std::regex_match(fname, match, v_regex)) {
            int ver = std::stoi(match[1].str());
            files.push_back({ver, fname, entry.path().string()});
        }
    }

    // 4. Sort numerically by version
    std::sort(files.begin(), files.end(), [](const MigrationFile& a, const MigrationFile& b) {
        return a.version < b.version;
    });

    int applied_count = 0;
    for (const auto& mf : files) {
        if (std::find(applied.begin(), applied.end(), mf.version) != applied.end()) {
            // Already applied
            continue;
        }

        spdlog::info("MigrationService: applying migration V{} ({})", mf.version, mf.filename);
        std::ifstream ifs(mf.full_path);
        if (!ifs.is_open()) {
            throw std::runtime_error("Could not open migration file: " + mf.full_path);
        }
        std::ostringstream ss;
        ss << ifs.rdbuf();
        std::string sql = ss.str();

        migration_repo_->ApplyMigration(mf.version, mf.filename, sql);
        applied_count++;
        spdlog::info("MigrationService: successfully applied migration V{}", mf.version);
    }

    return applied_count;
}

void MigrationService::ApplySeedDataIfFresh(const std::string& seed_file) {
    if (!fs::exists(seed_file)) {
        spdlog::warn("MigrationService: seed file '{}' not found", seed_file);
        return;
    }

    spdlog::info("MigrationService: executing seed data from '{}'", seed_file);
    std::ifstream ifs(seed_file);
    if (!ifs.is_open()) return;

    std::ostringstream ss;
    ss << ifs.rdbuf();
    std::string sql = ss.str();

    try {
        migration_repo_->ExecuteRawSql(sql);
        spdlog::info("MigrationService: seed data applied successfully");
    } catch (const std::exception& ex) {
        spdlog::warn("MigrationService: seed data notice: {}", ex.what());
    }
}

bool MigrationService::HealthCheck() {
    return migration_repo_->HealthCheck();
}

} // namespace faaliha::faalihamart::service
