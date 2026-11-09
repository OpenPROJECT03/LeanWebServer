#include "net/TcpConnection.h"

#include <string>

#include "net/Channel.h"
#include "net/EventLoop.h"

namespace lean_web_server::net {

void TcpConnection::on_http_request() {
    std::string body = "<html><body><h2>Hello, World!</h1></body></html>";
    send("HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body);
}

void TcpConnection::handle_read() {
    int saved_errno = 0;
    while (true) {
        int n = input_buffer_.read_fd(fd_, saved_errno);
        // 客户端连接关闭
        if (n == 0) {
            handle_close();
            return;
        }
        if (n < 0) {
            if (saved_errno == EAGAIN || saved_errno == EWOULDBLOCK) {
                break;
            }
            if (saved_errno == EINTR) {
                continue;
            }
            handle_error();
            return;
        }
    }

    // 解决粘包
    while (input_buffer_.read_available() > 0) {
        if (!http_context_.parse_request(input_buffer_)) {  // 解析失败，发送 400 响应并关闭连接
            send("HTTP/1.1 400 Bad Request\r\n\r\n");
            handle_close();
            return;
        }

        if (http_context_.parse_state() == http::HttpContext::HttpRequestParseState::kDone) {
            // 解析完成，处理请求
            on_http_request();
            input_buffer_.mark_used(http_context_.checked_bytes());
            http_context_.reset();
        } else {
            // 解析未完成，等待更多数据
            break;
        }
    }
}

void TcpConnection::handle_write() {
    if (!channel_->is_writing()) {  // 通道未准备好写入，直接返回
        return;
    }

    while (output_buffer_.read_available() > 0) {
        ssize_t write_byte = write(fd_, output_buffer_.read_ptr(), output_buffer_.read_available());
        if (write_byte < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                // 内核发送缓冲区已满，跳出循环
                break;
            }
            if (errno == EINTR) {
                continue;
            }
            handle_error();
            break;
        }

        // 标记写数据，更新缓冲区状态
        output_buffer_.mark_used(write_byte);
    }

    if (output_buffer_.read_available() == 0) {
        channel_->disable_write();
    }
}

void TcpConnection::send(const std::string& msg) {
    if (event_loop_->is_in_loop_thread()) {
        send_in_loop(msg);
    } else {
        event_loop_->run_in_loop([self = shared_from_this(), msg]() { self->send_in_loop(msg); });
    }
}

void TcpConnection::send_in_loop(const std::string& msg) {
    ssize_t write_byte = 0;

    // 非阻塞写，先尝试写入，失败则根据错误类型处理
    if (!channel_->is_writing() && output_buffer_.read_available() == 0) {
        write_byte = ::write(fd_, msg.data(), msg.size());
        if (write_byte < 0) {
            write_byte = 0;
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                handle_error();
                return;
            }
        }
    }

    // 发送剩余数据到缓冲区
    if (write_byte < static_cast<ssize_t>(msg.size())) {
        output_buffer_.append(msg.data() + write_byte,
                              static_cast<size_t>(msg.size() - write_byte));
        channel_->enable_write();
    }
}

}  // namespace lean_web_server::net
