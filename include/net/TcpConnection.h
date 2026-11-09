/**
 * @file TcpConnection.h
 * @brief TCP 连接
 * @author woruo03
 * @date 2026-11-04
 * @version 1.0
 */

#pragma once
#include <unistd.h>

#include <memory>

#include "net/Buffer.h"
#include "net/Channel.h"
#include "http/HttpContext.h"

namespace lean_web_server::net {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    using CloseCallBack = std::function<void(const std::shared_ptr<TcpConnection>)>;

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    TcpConnection(EventLoop* event_loop, int fd, uint64_t conn_id)
        : event_loop_(event_loop),
          fd_(fd),
          conn_id_(conn_id),
          channel_(std::make_unique<Channel>(event_loop, fd)) {
        channel_->set_read_callback([this]() { handle_read(); });
        channel_->set_write_callback([this]() { handle_write(); });
        channel_->set_error_callback([this]() { handle_error(); });
        channel_->set_close_callback([this]() { handle_close(); });
    }

    ~TcpConnection() { ::close(fd_); }

    // 连接建立时调用
    void connect_established() { channel_->enable_read(); }
    // 连接销毁时调用
    void connect_destroyed() { channel_->remove(); }
    // 设置关闭回调
    void set_close_callback(CloseCallBack cb) { close_callback_ = std::move(cb); }
    // 处理 HTTP 请求
    void on_http_request();

    int get_fd() const { return fd_; }
    uint64_t get_conn_id() const { return conn_id_; }
    EventLoop* get_event_loop() const { return event_loop_; }
    void send(const std::string& msg);

private:
    EventLoop* event_loop_;  // 事件循环
    int fd_;                 // 文件描述符
    uint64_t conn_id_;       // 连接 ID
    Buffer input_buffer_;    // 输入缓冲区
    Buffer output_buffer_;   // 输出缓冲区
    // 保证注册到 epoll 内核的指针具备绝对地址稳定性（Pointer
    // Stability），并实现头文件编译解耦与主动解绑。
    std::unique_ptr<Channel> channel_;
    http::HttpContext http_context_;  // HTTP 请求上下文

    CloseCallBack close_callback_;  // 关闭回调
    void send_in_loop(const std::string& msg);

    void handle_read();
    void handle_write();
    void handle_close() {
        if (close_callback_) {
            close_callback_(shared_from_this());
        }
    }
    void handle_error() { handle_close(); }
};

}  // namespace lean_web_server::net
