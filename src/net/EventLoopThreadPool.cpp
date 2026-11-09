#include "net/EventLoopThreadPool.h"

#include "net/EventLoop.h"

namespace lean_web_server::net {

void EventLoopThreadPool::start() {
    for (int i = 0; i < num_threads_; ++i) {
        threads_.emplace_back([this] {
            EventLoop loop;
            {
                std::scoped_lock<std::mutex> lock(mutex_);
                eventLoops_.emplace_back(&loop);
            }
            cv_.notify_one();  // 通知主线程有新的线程加入
            loop.loop();       // 进入事件循环
        });
    }
    std::unique_lock<std::mutex> lock(mutex_);  // 等待所有线程加入
    cv_.wait(lock, [this] { return eventLoops_.size() == static_cast<size_t>(num_threads_); });
}

EventLoop* EventLoopThreadPool::get_next_loop() {
    if (eventLoops_.empty()) {  // 线程池为空，返回主reactor
        return baseLoop_;
    }
    EventLoop* loop = eventLoops_[next_];
    next_ = (next_ + 1) % eventLoops_.size();
    return loop;
}

}  // namespace lean_web_server::net
