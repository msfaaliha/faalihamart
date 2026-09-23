#include <gtest/gtest.h>
#include <unordered_map>
#include "service/AuthService.h"
#include "repository/IUserRepository.h"
#include "exception/ApiException.h"

using namespace faaliha::faalihamart;

class FakeUserRepository : public repository::IUserRepository {
public:
    std::optional<model::User> FindById(int64_t id) override {
        auto it = users_by_id_.find(id);
        if (it != users_by_id_.end()) return it->second;
        return std::nullopt;
    }

    std::optional<model::User> FindByEmail(const std::string& email) override {
        auto it = users_by_email_.find(email);
        if (it != users_by_email_.end()) return it->second;
        return std::nullopt;
    }

    model::User Create(const model::User& user) override {
        model::User created = user;
        created.id_ = next_id_++;
        users_by_id_[created.id_] = created;
        users_by_email_[created.email_] = created;
        return created;
    }

    std::vector<model::User> FindAll() override {
        std::vector<model::User> list;
        for (const auto& [_, u] : users_by_id_) list.push_back(u);
        return list;
    }

    int64_t Count(std::optional<model::UserRole> /*role*/) override {
        return static_cast<int64_t>(users_by_id_.size());
    }

private:
    int64_t next_id_{1};
    std::unordered_map<int64_t, model::User> users_by_id_;
    std::unordered_map<std::string, model::User> users_by_email_;
};

TEST(AuthServiceTest, RegisterAndLoginFlow) {
    auto fake_repo = std::make_shared<FakeUserRepository>();
    service::AuthService auth_svc(fake_repo);

    dto::RegisterRequestDto reg_req;
    reg_req.name = "Test Buyer";
    reg_req.email = "buyer@test.com";
    reg_req.password = "SecurePass123!";
    reg_req.role = "BUYER";

    auto registered = auth_svc.Register(reg_req, "req-test");
    EXPECT_GT(registered.id, 0);
    EXPECT_EQ(registered.email, "buyer@test.com");
    EXPECT_EQ(registered.role, "BUYER");

    // Login with matching password
    dto::LoginRequestDto login_req;
    login_req.email = "buyer@test.com";
    login_req.password = "SecurePass123!";

    auto logged_in = auth_svc.Login(login_req, "req-test");
    EXPECT_EQ(logged_in.id, registered.id);

    // Login with incorrect password
    dto::LoginRequestDto bad_login;
    bad_login.email = "buyer@test.com";
    bad_login.password = "WrongPassword123!";
    EXPECT_THROW(auth_svc.Login(bad_login, "req-test"), exception::UnauthorizedException);
}

TEST(AuthServiceTest, DuplicateEmailThrowsConflict) {
    auto fake_repo = std::make_shared<FakeUserRepository>();
    service::AuthService auth_svc(fake_repo);

    dto::RegisterRequestDto reg;
    reg.name = "First User";
    reg.email = "duplicate@test.com";
    reg.password = "Password123!";
    reg.role = "BUYER";
    auth_svc.Register(reg, "req-test");

    // Same email registration
    dto::RegisterRequestDto reg2 = reg;
    reg2.name = "Second User";
    EXPECT_THROW(auth_svc.Register(reg2, "req-test"), exception::ConflictException);
}
