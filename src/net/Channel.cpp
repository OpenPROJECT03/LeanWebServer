#include "net/Channel.h"

#include "net/EventLoop.h"

namespace lean_web_server::net {

void Channel::handle_event() {
    if ((revents_ & EPOLLHUP) != 0 && (revents_ & EPOLLIN) == 0) {
        if (close_callback_) {
            close_callback_();
        }
        return;
    }

    if ((revents_ & EPOLLERR) != 0) {
        if (error_callback_) {
            error_callback_();
        }
        return;
    }

    if ((revents_ & (EPOLLIN | EPOLLRDHUP | EPOLLPRI)) != 0) {
        if (read_callback_) {
            read_callback_();
        }
    }

    if ((revents_ & EPOLLOUT) != 0) {
        if (write_callback_) {
            write_callback_();
        }
    }
}

void Channel::update() {
    loop_->update_channel(this);
}

void Channel::remove() {
    loop_->remove_channel(this);
}

}  // namespace lean_web_server::net
