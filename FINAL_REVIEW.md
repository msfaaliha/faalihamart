# FaalihaMart — Capstone Project Final Review Package

**Project Name**: FaalihaMart (`faaliha::faalihamart`)  
**Specification**: Native C++20 Marketplace Backend (Drogon · SQLite/PostgreSQL · libsodium · GoogleTest)  
**Evaluation Date**: 10 October 2026  
**Final Release Tag**: `v1.1.0`

---

## 1. Executive Summary & Final Deliverables Checklist

FaalihaMart satisfies all mandatory (F1–F8) and optional (O2–O4) requirements in the Capstone Specification:

| ID | Capstone Requirement | Status | Verification & Code References |
|---|---|---|---|
| **F1** | User registration & login (Buyer, Seller; Admin via seed migration) | **Complete** | [`AuthController`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/controller/AuthController.cpp), [`AuthService`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/AuthService.cpp), Argon2id via [`PasswordUtil`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/util/PasswordUtil.cpp). Cookie-backed session reissue on login. |
| **F2** | Seller: create, edit, delete product listings | **Complete** | [`ProductController`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/controller/ProductController.cpp), [`ProductService`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/ProductService.cpp). Seller ownership check enforced. |
| **F3** | Buyer: browse and search/filter products | **Complete** | `ProductController::ListProducts`, `SQLiteProductRepository::FindAll`. Category filters + parameterized `LIKE ?` search. |
| **F4** | Cart: add, update, remove items; running total | **Complete** | [`CartController`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/controller/CartController.cpp), [`CartService`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/CartService.cpp). Real-time inventory check and minor-unit running total. |
| **F5** | Checkout: place order via mock payment confirmation | **Complete** | [`OrderController::Checkout`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/controller/OrderController.cpp), [`OrderService::Checkout`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/OrderService.cpp). Strategy Pattern via [`IPaymentStrategy`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/payment/IPaymentStrategy.h), ACID transaction in repository. |
| **F6** | Order history: buyer past orders; seller incoming orders | **Complete** | `GET /api/v1/orders/buyer` and `GET /api/v1/orders/seller`. |
| **F7** | Admin: view all users and orders; moderate listings | **Complete** | [`AdminController`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/controller/AdminController.cpp), user table, order audit, platform revenue, and listing moderation table. |
| **F8** | Product reviews and star ratings on completed orders | **Complete** | [`ReviewController`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/controller/ReviewController.cpp), [`ReviewService`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/ReviewService.cpp). Verified purchase check (`DELIVERED` status required), 1–5 star ratings. |
| **O2** | Order status workflow (`PENDING` → `CONFIRMED` → `SHIPPED` → `DELIVERED`) | **Complete** | Status transitions enforced in [`OrderService::UpdateOrderStatus`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/OrderService.cpp). |
| **O3** | Seller sales dashboard (order counts, revenue totals) | **Complete** | `SellerController::GetDashboardStats`, `AdminService::GetSellerDashboardStats`. |
| **O4** | **AI Chatbot** (Gemini Live API + Mock Provider, rate limits, caching, UI) | **Complete** | [`ChatProvider`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/chat/ChatProvider.h), [`MockChatProvider`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/chat/MockChatProvider.cpp), [`GeminiChatProvider`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/chat/GeminiChatProvider.cpp), [`ChatService`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/service/ChatService.cpp), [`ChatController`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/src/controller/ChatController.cpp), floating widget. |
| **Deploy** | Production deployment (binary, systemd, Nginx, Docker) | **Complete** | Configured in [`deploy/`](file:///C:/Users/Asus/.gemini/antigravity/scratch/faalihamart/deploy), multi-stage Dockerfile, health check endpoint. |

---

## 2. Live Demonstration Sequence (Step-by-Step for Reviewers)

Follow this 9-step demonstration sequence to showcase the entire application:

### Step 1: Pre-seeded Account Switching & Authentication (F1)
1. Launch the preview server via `start_preview.bat` (or visit `http://localhost:8080/index.html`).
2. Notice the **Role Switcher** in the top navigation header:
   - `Buyer: Alice Smith` (ID 4)
   - `Buyer: Bob Jones` (ID 5)
   - `Seller: Apex Tech Gear` (ID 2)
   - `Seller: Urban Trendz` (ID 3)
   - `Admin: Faaliha` (ID 1)
3. Notice that selecting a Seller or Admin role dynamically unlocks the **Seller Dashboard** and **Admin Console** tabs in the navigation bar.

### Step 2: Product Catalog Browsing, Search & Filtering (F3)
1. Under **Browse Catalog**, click the category filter chips:
   - Click `Electronics` → displays Noise-Cancelling Headphones, Mechanical Keyboard, 4K Monitor, Vertical Mouse.
   - Click `Apparel` → displays Organic Cotton Hoodie, Commuter Backpack.
   - Click `Home & Living` → displays Soy Wax Candle Set, Stainless Steel Tumbler.
2. In the top search bar, type `monitor` or `cotton` → products filter instantaneously in real time.

### Step 3: Shopping Cart Operations (F4)
1. Click **+ Add to Cart** on the *Noise-Cancelling Wireless Headphones* (₹149.99).
2. Click **+ Add to Cart** on the *Classic Organic Cotton Hoodie* (₹49.99).
3. Click the **🛒 Cart** button in the header.
4. Modify quantities with `+` and `-`. Notice the **Running Total** updates dynamically in Indian Rupees minor units (cents) via `model::Money`.

### Step 4: Checkout & Mock Payment Confirmation (F5)
1. In the Cart modal, click **Proceed to Checkout**.
2. Enter delivery address: `124 Marketplace Avenue, Apt 4B, Chennai, India`.
3. Select `Mock Credit Card` or `Mock UPI`.
4. Click **Authorize & Confirm Order**.
5. The order is placed atomically: inventory is decremented, cart is cleared, and an order confirmation alert is displayed.

### Step 5: Buyer Order History & Status Progression (F6, O2)
1. Click the **📦 My Orders** tab.
2. Observe your newly placed order with status `PENDING`, along with existing past orders (`DELIVERED`, `SHIPPED`, `CONFIRMED`).
3. Notice the line items, quantities, and formatted monetary totals.

### Step 6: Verified Product Reviews & Star Ratings (F8)
1. In **My Orders**, locate order #1 (status `DELIVERED`).
2. Click the **⭐ Review** button next to a delivered item.
3. Select rating: `⭐⭐⭐⭐⭐ (5 - Outstanding)` and write: `"Exceptional sound quality and long battery life!"`.
4. Click **Submit Review**. The review appears on the product detail modal with verified purchase badge.
5. Note: Submitting a review on non-delivered orders is strictly blocked by the service layer.

### Step 7: Seller Dashboard & Sales Analytics (F2, O3)
1. In the role switcher, select `Seller: Apex Tech Gear`.
2. Click the **🏪 Seller Dashboard** tab.
3. Observe live metric cards: **Active Listings**, **Customer Orders**, and **Total Revenue** (in ₹).
4. Click **+ Add New Product** to create a new product listing (`Price: 59.99`, `Stock: 25`).
5. The new product appears immediately in your listings table and in the marketplace catalog.
6. Test editing a listing's price or deleting a product.

### Step 8: Admin Console, Revenue Audit & Listing Moderation (F7)
1. In the role switcher, select `Admin: Faaliha`.
2. Click the **⚙️ Admin Console** tab.
3. View platform metrics: Total Users, Active Sellers, Registered Buyers, Total Products, Total Orders, and Platform Gross Volume.
4. Inspect the **Registered Platform Users** table.
5. Inspect the **All Marketplace Orders** audit table.
6. Inspect the **Listing Moderation & Marketplace Oversight** table: click **Moderate / Remove** on any listing to remove it from the catalog with administrative authority.

### Step 9: AI Marketplace Chatbot (O4 / Sections 9 & 15)
1. Click the floating **💬 AI button** in the bottom-right corner.
2. The AI assistant opens with a welcome message and interactive suggestion chips.
3. Test domain FAQ queries:
   - Click `🎧 Electronics` → AI answers with current electronics listings and prices.
   - Click `📦 Track Order` → AI explains the `PENDING → CONFIRMED → SHIPPED → DELIVERED` lifecycle.
   - Click `💳 Payments` → AI explains mock credit card and mock UPI authorization.
   - Click `↩️ Returns` → AI explains the 7-day return policy and review requirements.
4. Test live typing: type `"Do you sell any hoodies?"` → AI responds with product details and price.
5. **Test In-Memory Caching (Section 9 Rule 5)**: Ask the identical question again → notice the response displays a blue **`CACHED`** badge, served instantaneously without duplicate network calls.
6. **Test Rate Limiter Guardrail (Section 9 Rule 4)**: The system enforces a strict 10 messages/minute sliding window per session to protect server and API quotas.

---

## 3. Architecture & Software Design Patterns

FaalihaMart demonstrates professional C++20 software engineering across all required design patterns:

1. **Repository Pattern**:
   - `IUserRepository`, `IProductRepository`, `ICartRepository`, `IOrderRepository`, `IReviewRepository`, `IMigrationRepository`.
   - Abstract interfaces with pure virtual methods; 100% parameterized SQLite implementations (`PrepareStatement` + `sqlite3_bind_*`).
2. **Front Controller Pattern**:
   - Drogon router dispatch coupled with `BaseController` for uniform request-ID extraction, session management, and `ApiResponse` error envelopes.
3. **Singleton Pattern**:
   - `DbPlugin::GetInstance()` manages connection pool lifecycle and SQLite WAL mode across process startup and teardown.
4. **Factory Pattern**:
   - `RepositoryFactory` creates repository instances (`CreateUserRepository(db)`, `CreateProductRepository(db)`, etc.).
5. **Strategy Pattern**:
   - `IPaymentStrategy` with swappable `MockPaymentStrategy` for payment verification.
   - `ChatProvider` with swappable `MockChatProvider` and `GeminiChatProvider`.
6. **Builder Pattern**:
   - Fluent builders in DTOs (`UserResponseDto::Builder`, `ProductResponseDto::Builder`).
7. **Dependency Injection**:
   - Constructor injection (`std::shared_ptr<Interface>`) across all services (`AuthService`, `ProductService`, `CartService`, `OrderService`, `ReviewService`, `AdminService`, `ChatService`).

---

## 4. Security & Quality Assurance Verification

- [x] **Zero SQL Injection**: 100% parameterized statements (`PrepareStatement` and `sqlite3_bind_*`).
- [x] **Secure Argon2id Hashing**: Handled via `libsodium` (`crypto_pwhash_str`). No plaintext passwords stored or logged.
- [x] **Safe Monetary Calculations**: Mandatory `model::Money` value type storing integer minor units (cents). Floating-point currency arithmetic is prohibited.
- [x] **Session Security**: Session regenerated upon login (`session->changeSessionIdToClient()`) to mitigate session fixation; explicit 1-hour idle timeout.
- [x] **Error Leaking Prevention**: `GlobalExceptionHandler` converts all runtime and validation exceptions to the fixed JSON envelope without leaking stack traces or internal paths.
- [x] **AI Guardrails**: Server-side API key retrieval (`std::getenv("GEMINI_API_KEY")`), input length cap (500 chars), per-session rate limit (10 msg/min), and graceful degraded static fallback.
- [x] **Sanitizers**: AddressSanitizer (`-fsanitize=address`) and UndefinedBehaviorSanitizer (`-fsanitize=undefined`) configured in CMake and CI.

---

## 5. Build, Test and Run Instructions

### Option 1: Instant Web & Chat Preview (Zero C++ Compilation)
```powershell
# From project directory
.\start_preview.bat
# Or in PowerShell:
powershell -ExecutionPolicy Bypass -File .\start_preview.ps1
```
Visit: `http://localhost:8080/index.html`

### Option 2: Native C++ Drogon Build (Linux / WSL / Ubuntu CI)
```bash
# 1. Install toolchain
sudo apt-get update && sudo apt-get install -y \
  build-essential cmake ninja-build libsqlite3-dev libsodium-dev \
  libssl-dev uuid-dev zlib1g-dev libgtest-dev libjsoncpp-dev \
  nlohmann-json3-dev libspdlog-dev

# 2. Build Drogon framework (if not installed)
git clone https://github.com/drogonframework/drogon.git /tmp/drogon
cd /tmp/drogon && git submodule update --init
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && sudo make install && sudo ldconfig

# 3. Configure & Compile FaalihaMart
cd /path/to/faalihamart
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 4. Run automated test suites
ctest --test-dir build --output-on-failure

# 5. Start Server
./build/faalihamart
```

### Option 3: Multi-Stage Docker Container
```bash
docker build -t faalihamart:latest -f deploy/Dockerfile .
docker run -p 8080:8080 -e AI_CHAT_PROVIDER=mock faalihamart:latest
```
Check health: `curl http://localhost:8080/api/v1/health`
