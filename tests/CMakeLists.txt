# tests/CMakeLists.txt

# 1. 优先使用系统安装的 GoogleTest
find_package(GTest CONFIG QUIET)
if(NOT TARGET GTest::gtest AND NOT TARGET GTest::GTest)
    find_package(GTest QUIET)
endif()

# 2. 如果未检测到，回退使用 FetchContent 自动下载构建
if(NOT TARGET GTest::gtest AND NOT TARGET GTest::GTest)
    message(
        STATUS
        "GoogleTest not found on system. Fetching via FetchContent..."
    )
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.18.0
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

    # 隔离第三方依赖，避免 clang-tidy 扫描 FetchContent 源码
    set(_SAVED_CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY}")
    set(CMAKE_CXX_CLANG_TIDY "")
    FetchContent_MakeAvailable(googletest)
    set(CMAKE_CXX_CLANG_TIDY "${_SAVED_CMAKE_CXX_CLANG_TIDY}")
endif()

# 3. 确定 GTest 目标标识
if(TARGET GTest::gtest_main)
    set(GTEST_MAIN_TARGET GTest::gtest_main)
elseif(TARGET GTest::Main)
    set(GTEST_MAIN_TARGET GTest::Main)
else()
    set(GTEST_MAIN_TARGET gtest_main)
endif()

# 4. 构建单元测试程序
add_executable(unit_tests test_core.cpp)

target_link_libraries(
    unit_tests
    PRIVATE
        LeanWebServer::core
        LeanWebServer::warnings
        LeanWebServer::sanitizers
        LeanWebServer::profiling
        Threads::Threads
        ${GTEST_MAIN_TARGET}
)

target_compile_features(unit_tests PRIVATE cxx_std_17)

# 5. 深度集成 CTest，自动发现并注册每一个测试用例
include(GoogleTest)
gtest_discover_tests(unit_tests)
