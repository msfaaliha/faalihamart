# Contributing to FaalihaMart

Thank you for contributing to **FaalihaMart** — the Native C++20 Marketplace Web Application.

---

## 1. Prerequisites

- **C++ Compiler**: GCC 12+, Clang 15+, or MSVC 2022+ with C++20 support.
- **Build System**: CMake ≥ 3.25, Ninja (recommended).
- **Package Manager**: [vcpkg](https://github.com/microsoft/vcpkg) in manifest mode.
- **Libraries**:
  - `drogon` (C++ HTTP Web Framework)
  - `sqlite3` (Embedded SQL database)
  - `libsodium` (Argon2id password hashing)
  - `nlohmann-json`
  - `spdlog` (Structured logging)
  - `gtest` (GoogleTest / GoogleMock)

---

## 2. Quickstart: Building & Running Locally

### Step 1: Clone Repository
```bash
git clone https://github.com/your-username/faalihamart.git
cd faalihamart
```

### Step 2: Install Dependencies via vcpkg
```bash
# If vcpkg is installed on your system:
vcpkg install
```

### Step 3: Configure CMake
```bash
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" \
  -DCMAKE_BUILD_TYPE=Release
```

### Step 4: Compile Project
```bash
cmake --build build --config Release
```

### Step 5: Run Automated Tests
```bash
ctest --test-dir build --output-on-failure
```

### Step 6: Start Server
```bash
cd build
./faalihamart
```

The application starts on `http://localhost:8080/index.html`.

---

## 3. Instant Web Preview (Zero C++ Compilation Required)

To inspect and test the interactive web frontend immediately without compiling the C++ binary:

- **Windows**: Double-click `start_preview.bat` or run:
  ```powershell
  powershell -ExecutionPolicy Bypass -File .\start_preview.ps1
  ```
- **Direct File**: Simply double-click `static/index.html` in your browser. The embedded frontend engine automatically runs in interactive demo mode with full seed data and local storage persistence.

---

## 4. Coding Standards

- **Style**: Follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
  - Types: `PascalCase`
  - Functions: `PascalCase`
  - Member variables: `snake_case_` (trailing underscore)
  - Constants: `kCamelCase`
  - Namespaces: `lower_snake_case` (`faaliha::faalihamart`)
- **SQL Security**: All queries **must** be strictly parameterized (`PrepareStatement` and `sqlite3_bind_*`). String interpolation in SQL is strictly forbidden.
- **Money Values**: All currency amounts must use the `model::Money` value type storing integer minor units (cents). Floating-point arithmetic on currency is forbidden.
- **Error Responses**: Must conform to the fixed JSON envelope `{ "success": false, "data": null, "error": { "code": "...", "message": "..." } }`. Never return HTTP 200 for error responses.
