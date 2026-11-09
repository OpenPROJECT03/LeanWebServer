
#include "net/Accepter.h"

#include <cerrno>

namespace lean_web_server::net {

void Accepter::handle_read() {
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = ::accept4(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr),
                                  &client_addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (client_fd <= -1) {
            // 非阻塞accept返回EAGAIN或EWOULDBLOCK时，退出循环
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            // 中断信号，继续循环
            if (errno == EINTR) {
                continue;
            }
            // 连接被重置，继续循环
            if (errno == ECONNABORTED) {
                continue;
            }
            // 其他错误，退出循环
            break;
        }

        if (new_connection_callback_) {  // 有设置连接回调，执行回调
            new_connection_callback_(client_fd);
        } else {  // 没有设置连接回调，直接关闭连接
            ::close(client_fd);
        }
    }
}

}  // namespace lean_web_server::net
