cmake_minimum_required(VERSION 3.25)
project(faalihamart VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings and options
if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Sanitizer options (Section 7)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

if(ENABLE_ASAN)
    if(MSVC)
        add_compile_options(/fsanitize=address)
    else()
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
endif()

if(ENABLE_UBSAN AND NOT MSVC)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

# Dependencies
find_package(Threads REQUIRED)
find_package(Drogon CONFIG QUIET)
find_package(nlohmann_json CONFIG QUIET)
find_package(spdlog CONFIG QUIET)
find_package(unofficial-sodium CONFIG QUIET)
find_package(SQLite3 QUIET)

# Fallback find modules if config mode not found
if(NOT Drogon_FOUND)
    find_package(Drogon QUIET)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Core Library Sources
file(GLOB_RECURSE LIB_SOURCES
    "src/model/*.cpp"
    "src/dto/*.cpp"
    "src/util/*.cpp"
    "src/exception/*.cpp"
    "src/filter/*.cpp"
    "src/plugin/*.cpp"
    "src/repository/*.cpp"
    "src/repository/sqlite/*.cpp"
    "src/service/*.cpp"
    "src/service/payment/*.cpp"
    "src/controller/*.cpp"
)

add_library(faalihamart_core STATIC ${LIB_SOURCES})

target_include_directories(faalihamart_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(TARGET Drogon::Drogon)
    target_link_libraries(faalihamart_core PUBLIC Drogon::Drogon)
endif()

if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(faalihamart_core PUBLIC nlohmann_json::nlohmann_json)
endif()

if(TARGET spdlog::spdlog)
    target_link_libraries(faalihamart_core PUBLIC spdlog::spdlog)
endif()

if(TARGET unofficial-sodium::sodium)
    target_link_libraries(faalihamart_core PUBLIC unofficial-sodium::sodium)
elseif(TARGET sodium)
    target_link_libraries(faalihamart_core PUBLIC sodium)
endif()

if(TARGET SQLite::SQLite3)
    target_link_libraries(faalihamart_core PUBLIC SQLite::SQLite3)
elseif(SQLite3_FOUND)
    target_link_libraries(faalihamart_core PUBLIC ${SQLite3_LIBRARIES})
    target_include_directories(faalihamart_core PUBLIC ${SQLite3_INCLUDE_DIRS})
endif()

target_link_libraries(faalihamart_core PUBLIC Threads::Threads)

# Main Server Executable
add_executable(faalihamart src/main.cpp)
target_link_libraries(faalihamart PRIVATE faalihamart_core)

# Static and DB resources copy
add_custom_command(TARGET faalihamart POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/static $<TARGET_FILE_DIR:faalihamart>/static
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/db $<TARGET_FILE_DIR:faalihamart>/db
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json $<TARGET_FILE_DIR:faalihamart>/config.json
    COMMENT "Copying static assets, database scripts, and configuration"
)

# Testing
enable_testing()
find_package(GTest QUIET)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
    add_subdirectory(test)
endif()
