# Tests CMake Configuration

find_package(GTest CONFIG QUIET)

if(TARGET GTest::gtest_main)
    file(GLOB TEST_SOURCES
        "unit/*.cpp"
        "repository/*.cpp"
    )

    add_executable(faalihamart_tests ${TEST_SOURCES})
    target_link_libraries(faalihamart_tests PRIVATE
        faalihamart_core
        GTest::gtest_main
        GTest::gmock
    )

    include(GoogleTest)
    gtest_discover_tests(faalihamart_tests)
else()
    message(STATUS "GoogleTest not found via find_package; test target disabled unless GTest is provided.")
endif()
