/**
 * @file Channel.h
 * @brief 通道类，用于管理事件循环中的文件描述符
 * @author woruo03
 * @date 2026-10-29
 * @version 1.0
 */

#pragma once
#include <sys/epoll.h>

#include <cstdint>
#include <functional>

namespace lean_web_server::net {
class EventLoop;

// 通道类，用于管理事件循环中的文件描述符
class Channel {
public:
    using EventCallBack = std::function<void()>;
    // 通道的状态
    enum class State : uint8_t {
        // 新创建的通道
        kNew,
        // 已添加到事件循环的通道
        kAdded,
        // 已从事件循环中删除的通道
        kDeleted,
    };

    Channel(EventLoop* loop, int fd) : loop_(loop), fd_(fd) {}
    ~Channel() = default;

    [[nodiscard]] int fd() const { return fd_; }
    [[nodiscard]] uint32_t events() const { return events_; }
    [[nodiscard]] uint32_t revents() const { return revents_; }
    [[nodiscard]] State state() const { return state_; }
    [[nodiscard]] bool is_writing() const { return (events_ & EPOLLOUT) != 0; }
    [[nodiscard]] bool is_reading() const { return (events_ & (EPOLLIN | EPOLLPRI)) != 0; }

    void set_revents(uint32_t revents) { revents_ = revents; }
    void set_state(State state) { state_ = state; }

    void enable_read() {
        if (!is_reading()) {
            events_ |= (EPOLLIN | EPOLLPRI | EPOLLET);
            update();
        }
    }
    void disable_read() {
        if (is_reading()) {
            events_ &= ~(EPOLLIN | EPOLLPRI | EPOLLET);
            update();
        }
    }
    void enable_write() {
        if (!is_writing()) {
            events_ |= EPOLLOUT;
            update();
        }
    }
    void disable_write() {
        if (is_writing()) {
            events_ &= ~EPOLLOUT;
            update();
        }
    }
    void disable_all() {
        events_ = 0;
        update();
    }

    void set_read_callback(EventCallBack cb) { read_callback_ = std::move(cb); }
    void set_write_callback(EventCallBack cb) { write_callback_ = std::move(cb); }
    void set_error_callback(EventCallBack cb) { error_callback_ = std::move(cb); }
    void set_close_callback(EventCallBack cb) { close_callback_ = std::move(cb); }

    // 处理事件
    void handle_event();
    // 移除通道
    void remove();

private:
    EventLoop* loop_;     // 所属的事件循环
    const int fd_;        // 文件描述符
    uint32_t events_{};   // 注册的事件
    uint32_t revents_{};  // 实际发生的事件
    State state_{};       // 状态

    EventCallBack read_callback_;   // 读回调
    EventCallBack write_callback_;  // 写回调
    EventCallBack error_callback_;  // 错误回调
    EventCallBack close_callback_;  // 关闭回调

    // 更新通道
    void update();
};

}  // namespace lean_web_server::net
