# FaalihaMart — Native C++20 Marketplace Backend

[![CI](https://github.com/your-username/faalihamart/actions/workflows/ci.yml/badge.svg)](.github/workflows/ci.yml)
[![Standard](https://img.shields.io/badge/c%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Framework](https://img.shields.io/badge/framework-Drogon-red.svg)](https://drogon.org)
[![Database](https://img.shields.io/badge/database-SQLite%20%7C%20PostgreSQL-green.svg)](https://sqlite.org)
[![Milestone](https://img.shields.io/badge/milestone-Sep%2021%20(2nd%20Review)-orange.svg)](#milestone-status)

**FaalihaMart** (namespace `faaliha::faalihamart`) is a high-performance multi-seller e-commerce marketplace web application served by a native C++20 HTTP backend using **Drogon**, SQLite/PostgreSQL, `spdlog`, and modern frontend technologies.

---

## 1. Quick Links: How to View the Website

You can view and interact with the full web marketplace immediately:

### Option A: Instant Browser Preview (Zero Compilation Required)
1. Navigate to `faalihamart/` in File Explorer or your terminal.
2. Double-click **`start_preview.bat`** (or execute `.\start_preview.ps1` in PowerShell).
3. The server starts at **`http://localhost:8080/index.html`** and opens your browser.
4. Alternatively, you can open **`static/index.html`** directly in any modern browser. The application includes a self-contained interactive state engine with pre-seeded demo accounts!

### Option B: Native C++ Drogon HTTP Server
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/faalihamart
```
Visit `http://localhost:8080/index.html`.

---

## 2. Milestone Deliverables (Up to September 21 / 2nd Review)

| ID | Feature Requirement | Status | Verification |
|---|---|---|---|
| **F1** | User registration and login (Buyer, Seller; Admin via seed row) | Completed | Session auth, Argon2id hashing, role enforcement |
| **F2** | Seller: create, edit, delete product listings | Completed | Full CRUD, ownership check, stock management |
| **F3** | Buyer: browse and search/filter products | Completed | Category filtering, keyword search, price formatting |
| **F4** | Cart: add, update, remove items; running total | Completed | Stock validation, dynamic totals, running total display |
| **F5** | Checkout: place order from cart via mock payment | Completed | Mock payment confirmation, ACID transaction, stock decrement |
| **F6** | Order history: buyer past orders; seller incoming orders | Completed | Order status tracking, order line items |
| **F7** | Admin: view all users, orders; moderate listings | Completed | Admin dashboard, user table, platform revenue |
| **F8** | Product reviews and star ratings on completed orders | Completed | Verification of DELIVERED status, 1-5 star ratings |
| **O2** | Order status workflow (`PENDING` → `CONFIRMED` → `SHIPPED` → `DELIVERED`) | Completed | Dynamic status progression, seller controls |
| **O3** | Seller sales dashboard (order counts, revenue totals) | Completed | Revenue calculation, active vs completed order metrics |
| **Deploy** | Docker multi-stage build, systemd service, Nginx reverse proxy | Completed | Production deployment bundle in `deploy/` |

---

## 3. Architecture Specification

FaalihaMart strictly follows the layered architecture specified in Section 2:

```
Browser (HTML/CSS/Vanilla JS + fetch AJAX)
   |
   v
Filter Layer (drogon::HttpFilter subclasses)
   ├── LoggingFilter (Request-ID generation & structured logging via spdlog)
   ├── AuthFilter (Cookie-backed session checks & role authorization)
   └── CORSFilter (Cross-origin preflight handling)
   |
   v
Router Layer (Drogon built-in router dispatches to HttpController subclasses)
   |
   v
Controller Layer (BaseController, AuthController, ProductController, etc.)
   └── Thin controllers: HTTP orchestration only, no SQL, no business logic
   |
   v
Service Layer (AuthService, ProductService, CartService, OrderService, etc.)
   └── Pure C++ business logic & validation (ValidationUtil). No direct DB calls.
   |
   v
Repository Layer (IUserRepository, IProductRepository, etc.)
   └── Pure virtual interfaces + SQLite/Postgres implementations. 100% parameterized SQL.
   |
   v
Connection Pool & Migration Runner (owned by DbPlugin, initialized once at startup)
   └── SQLite WAL mode (dev/test) / PostgreSQL 15+ (production)
```

---

## 4. Design Patterns Implemented (Section 10)

1. **Repository Pattern**: Data access abstraction behind pure virtual interfaces (`IProductRepository`, `IOrderRepository`, etc.).
2. **Front Controller Pattern**: Drogon router coupled with `BaseController` for uniform error translation and session extraction.
3. **Singleton Pattern**: Database connection pool and lifecycle owned by `DbPlugin`, exposed via thread-safe accessor.
4. **Factory Pattern**: `RepositoryFactory` for instantiating repository implementations.
5. **Strategy Pattern**: `IPaymentStrategy` with `MockPaymentStrategy` for swappable payment processing.
6. **Builder Pattern**: Fluent builders in DTOs (`UserResponseDto::Builder`, `ProductResponseDto::Builder`).
7. **Dependency Injection**: Manual constructor injection across all services (`std::shared_ptr<IRepository>`).

---

## 5. Security Checklist Compliance (Section 7)

- [x] **Every query parameterized**: All SQL statements use `PrepareStatement` and `sqlite3_bind_*`. Zero string concatenation.
- [x] **Argon2id password hashing**: Implemented via `PasswordUtil` (`libsodium crypto_pwhash_str`). No plaintext passwords stored or logged.
- [x] **Session management**: Handled via Drogon's session engine. Pre-auth sessions discarded and reissued upon login; explicit 1-hour idle timeout.
- [x] **Escaped output**: All user-supplied output safely handled; DTOs strictly separate from entities (`UserResponseDto` never contains `password_hash`).
- [x] **Global exception handling**: All runtime and validation exceptions translated to the fixed JSON envelope:
  ```json
  { "success": false, "data": null, "error": { "code": "VALIDATION_ERROR", "message": "...", "details": [] } }
  ```
- [x] **Credentials security**: `.env.example` committed; real `.env` and `config.json` excluded in `.gitignore`.
- [x] **Sanitizers**: AddressSanitizer and UndefinedBehaviorSanitizer integrated into CMake and CI.

---

## 6. Pre-seeded Demo Accounts

| Role | Email | Password | Purpose |
|---|---|---|---|
| **Admin** | `admin@faalihamart.com` | `AdminPass123!` | System oversight, user auditing, listing moderation |
| **Seller 1** | `techseller@faalihamart.com` | `SellerPass123!` | Electronics listings, order fulfillment |
| **Seller 2** | `styleseller@faalihamart.com` | `SellerPass123!` | Apparel and Home listings |
| **Buyer 1** | `alice@faalihamart.com` | `BuyerPass123!` | Browse, shopping cart, checkout, reviews |
| **Buyer 2** | `bob@faalihamart.com` | `BuyerPass123!` | Browse and order placement |

---

## 7. Testing Strategy (Section 6)

Run automated unit and repository tests with:
```bash
ctest --test-dir build --output-on-failure
```
Test suites include:
- `MoneyTest`: Monetary value type safety, integer minor unit arithmetic.
- `ValidationUtilTest`: Field-level validation and regex checks.
- `AuthServiceTest`: User registration, login verification, duplicate email conflict handling.
- `ProductServiceTest`: Ownership checks and seller permissions.
- `CartServiceTest`: Inventory limits and running totals.
- `SQLiteUserRepositoryTest` & `SQLiteProductRepositoryTest`: Real parameterized SQL execution against `:memory:` SQLite.
