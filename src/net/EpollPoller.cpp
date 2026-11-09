#include "net/EpollPoller.h"

#include <cerrno>

#include "net/Channel.h"

namespace lean_web_server::net {

void EpollPoller::poll(std::vector<Channel*>& active_channels, int timeout) {
    int n = ::epoll_wait(epoll_fd_, events_.data(), static_cast<int>(events_.size()), timeout);
    if (n == -1) {
        if (errno != EINTR) {
            // perror("epoll_wait error");
            // [FIXME]: 忽略 EINTR 错误
        }
        return;
    }

    for (int i = 0; i < n; ++i) {
        auto* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        active_channels.push_back(channel);
    }
    if (n == static_cast<int>(events_.size())) {
        events_.resize(events_.size() * 2);
    }
}

void EpollPoller::update_channel(Channel* channel) const {
    int fd = channel->fd();

    struct epoll_event event{};
    event.data.ptr = channel;
    event.events = channel->events();
    if (channel->state() == Channel::State::kNew || channel->state() == Channel::State::kDeleted) {
        ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
        channel->set_state(Channel::State::kAdded);
    } else {
        if (channel->events() == 0) {
            ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
            channel->set_state(Channel::State::kDeleted);
        } else {
            ::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
        }
    }
}

void EpollPoller::remove_channel(Channel* channel) const {
    int fd = channel->fd();
    if (channel->state() == Channel::State::kAdded) {
        ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
    }
    channel->set_state(Channel::State::kNew);
}

}  // namespace lean_web_server::net
