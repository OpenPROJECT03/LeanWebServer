#include <csignal>

#include "net/EventLoop.h"
#include "http/HttpServer.h"

int main() {
    // 忽略 SIGPIPE，防止客户端断开连接时因写套接字导致进程意外退出
    std::signal(SIGPIPE, SIG_IGN);

    // 创建事件循环和 HTTP 服务器
    lean_web_server::net::EventLoop main_loop;
    lean_web_server::http::HttpServer http_server(&main_loop, 9006, 15);

    // 启动 HTTP 服务器
    http_server.start();

    // 运行主事件循环
    main_loop.loop();

    return 0;
}
