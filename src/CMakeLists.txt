# src/CMakeLists.txt

add_library(
    lean_web_server_core
    http/HttpContext.cpp
    http/HttpServer.cpp
    net/Accepter.cpp
    net/Buffer.cpp
    net/Channel.cpp
    net/EpollPoller.cpp
    net/EventLoop.cpp
    net/EventLoopThreadPool.cpp
    net/TcpConnection.cpp
)
add_library(LeanWebServer::core ALIAS lean_web_server_core)

target_compile_features(lean_web_server_core PUBLIC cxx_std_17)

target_include_directories(
    lean_web_server_core
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    lean_web_server_core
    PUBLIC Threads::Threads
    PRIVATE
        LeanWebServer::warnings
        LeanWebServer::sanitizers
        LeanWebServer::profiling
)
