/**
 * @file EpollPoller.h
 * @brief Epoll 轮询器类
 * @author woruo03
 * @date 2026-10-29
 * @version 1.0
 */

#pragma once
#include <sys/epoll.h>
#include <unistd.h>

#include <vector>

namespace lean_web_server::net {

class Channel;

// Epoll 轮询器类
class EpollPoller {
public:
    EpollPoller() : epoll_fd_(::epoll_create1(EPOLL_CLOEXEC)), events_(16) {}
    ~EpollPoller() { ::close(epoll_fd_); }

    // 轮询事件
    void poll(std::vector<Channel*>& active_channels, int timeout = -1);
    // 更新通道
    void update_channel(Channel* channel) const;
    // 删除通道
    void remove_channel(Channel* channel) const;

private:
    int epoll_fd_;                            // epoll 句柄
    std::vector<struct epoll_event> events_;  // epoll 事件数组
};

}  // namespace lean_web_server::net
