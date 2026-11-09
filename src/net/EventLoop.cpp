#include "net/EventLoop.h"

namespace lean_web_server::net {

void EventLoop::loop() {
    looping_ = true;
    while (looping_) {
        active_channel_.clear();
        poller_->poll(active_channel_);
        for (Channel* channel : active_channel_) {
            channel->handle_event();
        }
        do_pending_functors();
    }
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    ::write(wakeup_fd_, &one, sizeof(one));
}

void EventLoop::handle_wakeup() const {
    uint64_t one = 1;
    ::read(wakeup_fd_, &one, sizeof(one));
}

void EventLoop::run_in_loop(Functor cb) {
    if (is_in_loop_thread()) {  // 如果是循环线程，直接执行回调
        cb();
    } else {  // 否则将回调加入待执行队列
        queue_in_loop(std::move(cb));
    }
}

void EventLoop::queue_in_loop(Functor cb) {
    {
        std::scoped_lock lock(mutex_);
        pending_functors_.push_back(std::move(cb));
    }
    wakeup();
}

void EventLoop::do_pending_functors() {
    std::vector<Functor> temp;
    {
        // 交换 pending_functors_ 与 temp，避免锁持有时间过长
        std::scoped_lock lock(mutex_);
        pending_functors_.swap(temp);
        pending_functors_.reserve(1024);
    }

    // 执行待执行的函数
    for (Functor& functor : temp) {
        functor();
    }
}

}  // namespace lean_web_server::net
