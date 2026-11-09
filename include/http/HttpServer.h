/**
 * @file HttpServer.h
 * @brief HttpServer 类，负责监听和处理 HTTP 连接
 * @author woruo03
 * @date 2026-11-06
 * @version 1.0
 */

#pragma once
#include <cstdint>
#include <unordered_map>

#include "net/Accepter.h"
#include "net/EventLoopThreadPool.h"
#include "net/TcpConnection.h"

namespace lean_web_server::http {

class HttpServer {
public:
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    HttpServer(net::EventLoop* event_loop, uint16_t port, int num_threads)
        : main_loop_(event_loop),
          accepter_(std::make_unique<net::Accepter>(event_loop, port)),
          event_loop_thread_pool_(std::make_unique<net::EventLoopThreadPool>(event_loop, num_threads)) {
        accepter_->set_new_connection_callback([this](int sockfd) { on_new_connection(sockfd); });
    }

    // 启动服务器，开始监听连接
    void start();

private:
    net::EventLoop* main_loop_;  // 主事件循环
    uint64_t conn_id_{};    // 连接 ID
    std::unique_ptr<net::Accepter> accepter_;
    std::unique_ptr<net::EventLoopThreadPool> event_loop_thread_pool_;          // 事件循环线程池
    std::unordered_map<int, std::shared_ptr<net::TcpConnection>> connections_;  // 连接映射

    // 处理新连接
    void on_new_connection(int sockfd);
};

}  // namespace lean_web_server::net
