/**
 * @file EventLoopThreadPool.h
 * @brief 事件循环线程池
 * @author woruo03
 * @date 2026-10-31
 * @version 1.0
 */

#pragma once
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace lean_web_server::net {

class EventLoop;

class EventLoopThreadPool {
public:
    EventLoopThreadPool(EventLoop* baseLoop, int num_threads)
        : num_threads_(num_threads), baseLoop_(baseLoop) {}

    // 启动线程池，创建指定数量的线程
    void start();
    // 获取下一个事件循环
    EventLoop* get_next_loop();

private:
    size_t next_{};                       // 下一个事件循环的索引
    int num_threads_;                     // 线程池中的线程数量
    EventLoop* baseLoop_;                 // 主reactor
    std::mutex mutex_;                    // 互斥锁
    std::condition_variable cv_;          // 条件变量
    std::vector<EventLoop*> eventLoops_;  // 事件循环列表
    std::vector<std::thread> threads_;    // 线程列表
};

}  // namespace lean_web_server::net
