cmake_minimum_required(VERSION 3.25)

# 定义工程元信息及语言
project(
    LeanWebServer
    VERSION 0.1.0
    DESCRIPTION "Lightweight Modern C++ Web Server"
    HOMEPAGE_URL "https://github.com/OpenPROJECT03/LeanWebServer"
    LANGUAGES CXX
)

# 强制/推荐 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 仅顶层项目生效的开发环境配置
if(PROJECT_IS_TOP_LEVEL)
    # 默认生成 compile_commands.json，供 clangd / VSCode / Neovim 读取
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # 默认构建类型防御性设置（仅对单配置生成器生效，如 Ninja / Make）
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE
            "RelWithDebInfo"
            CACHE STRING
            "Choose Debug/Release/RelWithDebInfo/MinSizeRel"
            FORCE
        )
    endif()
endif()

# 功能特性与构建选项
option(
    LEAN_WEB_SERVER_BUILD_APPS
    "Build the web server executable application"
    ${PROJECT_IS_TOP_LEVEL}
)
option(
    LEAN_WEB_SERVER_BUILD_TESTS
    "Build the testing suite"
    ${PROJECT_IS_TOP_LEVEL}
)
option(LEAN_WEB_SERVER_ENABLE_WARNINGS "Enable strict compiler warnings" ON)
option(
    LEAN_WEB_SERVER_WARNINGS_AS_ERRORS
    "Treat compiler warnings as errors"
    OFF
)
option(
    LEAN_WEB_SERVER_ENABLE_SANITIZERS
    "Enable runtime sanitizers (ASan, UBSan)"
    OFF
)
option(
    LEAN_WEB_SERVER_ENABLE_PROFILING
    "Enable profiling optimizations and preserve frame pointers"
    OFF
)
option(
    LEAN_WEB_SERVER_ENABLE_CLANG_TIDY
    "Enable clang-tidy static analysis"
    OFF
)
option(
    LEAN_WEB_SERVER_INSTALL
    "Enable minimal installation of the server executable"
    ${PROJECT_IS_TOP_LEVEL}
)

# 标准安装路径支持
include(GNUInstallDirs)

# 查找必需的系统多线程依赖 (POSIX threads)
find_package(Threads REQUIRED)

# 模块化配置：引入 cmake/ 目录中的辅助模块
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)
include(Profiling)
include(StaticAnalyzers)

# 自动生成版本头文件 (写入构建目录，保持源码树干净)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp"
    @ONLY
)

# 引入核心库子目录
add_subdirectory(src)

# 引入可执行程序子目录
if(LEAN_WEB_SERVER_BUILD_APPS)
    add_subdirectory(app)
endif()

# 引入测试套件子目录
if(LEAN_WEB_SERVER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 自动化代码格式化目标 (ninja format / cmake --build build --target format)
if(PROJECT_IS_TOP_LEVEL)
    find_program(CLANG_FORMAT_EXE NAMES clang-format)
    if(CLANG_FORMAT_EXE)
        file(
            GLOB_RECURSE ALL_SOURCE_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        )
        add_custom_target(
            format
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCE_FILES}
            COMMENT "Formatting all C++ source files with clang-format"
        )
    endif()
endif()
