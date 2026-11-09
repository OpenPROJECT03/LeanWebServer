/**
 * @file EventLoop.h
 * @brief 事件循环类，用于管理事件循环中的文件描述符
 * @author woruo03
 * @date 2026-10-29
 * @version 1.0
 */

#pragma once
#include <sys/eventfd.h>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "net/Channel.h"
#include "net/EpollPoller.h"

namespace lean_web_server::net {

class EventLoop {
public:
    using Functor = std::function<void()>;

    EventLoop()
        : thread_id_(std::this_thread::get_id()),
          looping_(false),
          poller_(std::make_unique<EpollPoller>()),
          active_channel_(0),
          wakeup_fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
          wakeup_channel_(std::make_unique<Channel>(this, wakeup_fd_)) {
        wakeup_channel_->set_read_callback([this] { handle_wakeup(); });
        wakeup_channel_->enable_read();
    }

    ~EventLoop() {
        wakeup_channel_->remove();
        ::close(wakeup_fd_);
    }

    void loop();                     // 时间循环
    void run_in_loop(Functor cb);    // 运行在循环线程中的回调
    void queue_in_loop(Functor cb);  // 将回调函数加入待执行队列

    // 更新通道
    void update_channel(Channel* channel) { poller_->update_channel(channel); }
    // 移除通道
    void remove_channel(Channel* channel) { poller_->remove_channel(channel); }
    [[nodiscard]] bool is_in_loop_thread() const {
        return thread_id_ == std::this_thread::get_id();
    }

private:
    const std::thread::id thread_id_;          // 线程 ID
    std::atomic<bool> looping_;                // 循环标志
    std::unique_ptr<EpollPoller> poller_;      // 轮询器
    std::vector<Channel*> active_channel_;     // 活跃通道
    int wakeup_fd_;                            // 唤醒文件描述符
    std::unique_ptr<Channel> wakeup_channel_;  // 唤醒通道

    std::mutex mutex_;                       // 互斥锁
    std::vector<Functor> pending_functors_;  // 待执行的函数

    void wakeup() const;
    void handle_wakeup() const;
    void do_pending_functors();
};

}  // namespace lean_web_server::net
