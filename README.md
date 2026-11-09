# LeanWebServer — 轻量级高性能现代 C++ Web 服务器

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white" alt="C++17"/>
  <img src="https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black" alt="Linux"/>
  <img src="https://img.shields.io/badge/CMake-3.25%2B-064F8C?logo=cmake&logoColor=white" alt="CMake"/>
  <img src="https://img.shields.io/badge/build-Ninja-brightgreen" alt="Ninja"/>
  <img src="https://img.shields.io/badge/license-Apache%202.0-blue" alt="Apache 2.0 License"/>
</p>

基于 **C++17** 的轻量级 Web 服务器，参考 TinyWebServer 用现代 C++ 与现代 CMake 工程实践重写。网络层采用 **epoll 主从 Reactor（Main-Sub Reactors / One Loop Per Thread）** 模型，HTTP 层采用状态机式请求解析，配套完整的质量工具链（告警策略、Sanitizers、剖析、静态检查、单元测试）。

---

## 项目状态

网络层与 HTTP 请求解析已就绪，后续能力持续推进中。

### 已实现

- **网络层**（`src/net/`）
    - 事件驱动核心：事件循环、epoll 轮询器、通道抽象，支持边缘触发 EPOLLET + 非阻塞 I/O
    - 连接管理：连接接受器、TCP 连接（`std::shared_ptr` 生命周期管理）
    - 缓冲区：基于 `std::vector<char>` 的动态扩容读写缓冲区
    - 线程池：Round-Robin 轮询分发的 IO 线程池

- **HTTP 层**（`src/http/`）
    - 服务器主体：Main Reactor 专职 `accept`，新连接轮询派发到 IO 线程
    - 请求解析：状态机式 HTTP 请求解析上下文

---

## 核心特性

- **现代 C++（C++17）**：`std::string_view`、智能指针连接管理、RAII 资源管控、CMake 元编程辅助模块。
- **主从 Reactor 并发模型**：Main Reactor 专职 `accept`，IO 线程池各持一个独立事件循环处理已连接套接字，通过 `run_in_loop` / `queue_in_loop` 实现无锁跨线程调度。
- **现代 CMake 目标化工程**：命名空间别名目标（`LeanWebServer::core` / `LeanWebServer::warnings` 等），`INTERFACE` 库承载编译选项，完全 target-based，无全局变量污染。
- **严格质量保障**：
    - 模块化编译器告警策略，支持一键 `-Werror`
    - ASan + UBSan 运行时安全检查
    - perf / 火焰图剖析优化配置
    - clang-tidy 静态检查 + clang-format 统一格式化
    - GoogleTest 单元测试 + CTest 自动注册（`gtest_discover_tests`）
- **构建预设（CMake Presets）**：开箱即用支持 `dev`、`release`、`asan`、`perf` 预设，绑定 `clang++` + Ninja。
- **自动版本注入**：版本头文件模板于配置期生成到构建目录，源码树保持干净。

---

## 目录结构

```text
.
├── CMakeLists.txt              # 根构建配置
├── CMakePresets.json           # 多构建模式预设（dev / release / asan / perf）
├── .clang-format               # 统一代码风格规则
├── .clang-tidy                 # 静态检查规则集
├── LICENSE
├── README.md
├── cmake/                      # 模块化 CMake 辅助模块
│   ├── CompilerWarnings.cmake  # 编译器告警策略与 -Werror 开关
│   ├── Sanitizers.cmake        # 内存与未定义行为运行时检查
│   ├── Profiling.cmake         # 性能剖析构建配置
│   └── StaticAnalyzers.cmake   # 静态分析工具集成
├── include/                    # 公共头文件
│   ├── net/                    # 事件驱动网络层所有组件的接口定义
│   ├── http/                   # HTTP 协议处理层接口定义
│   ├── base/                   # 基础设施层接口（规划中）
│   ├── db/                     # 数据访问层接口（规划中）
│   └── version.hpp.in          # 版本信息模板，构建时自动生成到构建目录
├── src/                        # 核心库源码（目标：LeanWebServer::core）
│   ├── net/                    # 网络层实现
│   ├── http/                   # HTTP 层实现
│   ├── base/                   # 基础设施实现（规划中）
│   └── db/                     # 数据访问层实现（规划中）
├── app/                        # 服务器可执行程序入口
├── tests/                      # 单元测试（GoogleTest + CTest）
├── docs/                       # 架构与详细设计文档
└── plan/                       # 开发计划
```

> 生成的版本头文件与 `compile_commands.json` 均在构建目录（`build/<preset>/`）中，源码树保持干净。

---

## 环境要求

| 项目       | 要求                                                                   |
| ---------- | ---------------------------------------------------------------------- |
| 操作系统   | Linux（推荐主流较新发行版）                                            |
| C++ 编译器 | `clang++`（推荐）或 `g++`，需支持 C++17                                |
| 构建系统   | Ninja（推荐）或 Make；CMake ≥ 3.25                                     |
| 可选工具   | `clang-format`（格式化）、`clang-tidy`（静态检查）、`perf`（性能剖析） |

> 未显式指定构建类型时，单配置生成器默认使用 `RelWithDebInfo`。

---

## 快速上手

### 1. 开发构建与运行

```bash
# 配置 Debug 开发环境（生成 build/dev）
cmake --preset dev

# 编译所有目标
cmake --build --preset dev

# 运行服务器（默认监听 9006 端口）
./build/dev/app/lean_web_server

# 验证端口连通性
curl -v http://localhost:9006/
```

> 当前 HTTP 响应生成尚未实现，`curl` 可能拿不到完整响应，属预期行为。  
> 可用 `nc localhost 9006` 手工发送请求文本观察解析行为。

### 2. 运行单元测试

`dev` / `release` / `asan` / `perf` 预设默认**关闭**测试构建，需要时在配置期显式开启：

```bash
cmake --preset dev -DLEAN_WEB_SERVER_BUILD_TESTS=ON
cmake --build --preset dev
ctest --preset dev
```

> 当前测试用例为占位注释，`ctest` 会报告 "No tests were found"，属预期行为；补充用例后即可被自动注册。  
> GoogleTest 优先使用系统安装版本，未检测到时自动经 `FetchContent` 下载构建（首次开启可能需要联网）。

### 3. 内存安全检查构建（ASan + UBSan）

```bash
cmake --preset asan
cmake --build --preset asan

# 带安全检查运行测试（需按上一节开启测试构建）
ctest --preset asan
```

### 4. Release 优化构建

```bash
cmake --preset release
cmake --build --preset release
```

### 5. 性能剖析与火焰图构建

专为 `perf` 采样与火焰图设计：开启最高优化（`-O3`）同时保留帧指针（`-fno-omit-frame-pointer`）、禁用尾调用消除（`-fno-optimize-sibling-calls`）并生成完整调试符号（`-g`）：

```bash
cmake --preset perf
cmake --build --preset perf

# 启动服务并采样
./build/perf/app/lean_web_server &
SERVER_PID=$!

# 使用 Linux perf 采集调用栈数据（99Hz 采样率）
perf record -F 99 -g -p $SERVER_PID -- sleep 30

# 生成火焰图（配合 FlameGraph 脚本）
perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > flamegraph.svg
```

### 6. 极简二进制安装

```bash
# 安装可执行程序到指定目录
cmake --install build/dev --prefix /path/to/install
```

安装后的文件布局极简干净：

```text
/path/to/install/
└── bin/
    └── lean_web_server
```

---

## 构建选项

| 选项                                 | 默认值                  | 说明                                  |
| ------------------------------------ | ----------------------- | ------------------------------------- |
| `LEAN_WEB_SERVER_BUILD_APPS`         | 顶层构建 `ON`           | 构建可执行程序                        |
| `LEAN_WEB_SERVER_BUILD_TESTS`        | 顶层 `ON`，预设中 `OFF` | 构建单元测试套件                      |
| `LEAN_WEB_SERVER_ENABLE_WARNINGS`    | `ON`                    | 启用严格编译器告警                    |
| `LEAN_WEB_SERVER_WARNINGS_AS_ERRORS` | `OFF`                   | 告警视为错误（`-Werror`）             |
| `LEAN_WEB_SERVER_ENABLE_SANITIZERS`  | `OFF`                   | 启用 ASan + UBSan                     |
| `LEAN_WEB_SERVER_ENABLE_PROFILING`   | `OFF`                   | 启用剖析优化（保留帧指针 + 调试符号） |
| `LEAN_WEB_SERVER_ENABLE_CLANG_TIDY`  | `OFF`                   | 构建时运行 clang-tidy                 |
| `LEAN_WEB_SERVER_INSTALL`            | 顶层构建 `ON`           | 启用 `cmake --install` 安装规则       |

> 作为子项目（`add_subdirectory`）引入时，`*_APPS` / `*_TESTS` / `*_INSTALL` 默认关闭，不会污染宿主工程。

---

## 代码质量工具

```bash
# 一键格式化全部源码（需安装 clang-format，遵循 .clang-format）
cmake --build build/dev --target format

# 构建期静态检查（需安装 clang-tidy，遵循 .clang-tidy）
cmake --preset dev -DLEAN_WEB_SERVER_ENABLE_CLANG_TIDY=ON
cmake --build --preset dev
```

---

## 许可证

见 [LICENSE](LICENSE)。
