/**
 * @file Accepter.h
 * @brief 监听套接字的独立封装组件
 * @author woruo03
 * @date 2026-11-05
 * @version 1.0
 */

#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>

#include "net/Channel.h"

namespace lean_web_server::net {

class EventLoop;

class Accepter {
public:
    using NewConnectionCallback = std::function<void(int sockfd)>;

    Accepter(EventLoop* event_loop, uint16_t port)
        :  // event_loop_(event_loop),
           // 创建监听套接字，设置非阻塞和关闭执行选项
          listen_fd_(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)),
          accept_channel_(event_loop, listen_fd_) {
        int opt_val = 1, ret = 0;
        // 设置 SO_REUSEADDR 选项，允许地址重用
        ret = ::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
        if (ret == -1) {
            ::close(listen_fd_);
            return;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        // 绑定监听套接字到指定端口
        ret = ::bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if (ret == -1) {
            ::close(listen_fd_);
            return;
        }

        // 监听端口，等待连接
        ret = ::listen(listen_fd_, SOMAXCONN);
        if (ret == -1) {
            ::close(listen_fd_);
            return;
        }

        // 设置读取回调
        accept_channel_.set_read_callback([this] { handle_read(); });
    }

    // 启动监听，使通道变为可读状态
    void listen() { accept_channel_.enable_read(); }

    void set_new_connection_callback(NewConnectionCallback cb) {
        new_connection_callback_ = std::move(cb);
    }

private:
    // EventLoop* event_loop_;
    int listen_fd_;
    Channel accept_channel_;
    NewConnectionCallback new_connection_callback_;

    // 处理读事件，接受新连接
    void handle_read();
};

}  // namespace lean_web_server::net
