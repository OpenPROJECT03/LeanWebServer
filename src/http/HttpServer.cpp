#include "http/HttpServer.h"

#include "net/EventLoop.h"

namespace lean_web_server::http {

void HttpServer::start() {
    event_loop_thread_pool_->start();
    // accepter_->listen();
    main_loop_->run_in_loop([this] { accepter_->listen(); });
}

void HttpServer::on_new_connection(int sockfd) {
    net::EventLoop* io_loop = event_loop_thread_pool_->get_next_loop();
    auto conn = std::make_shared<net::TcpConnection>(io_loop, sockfd, ++conn_id_);
    connections_[sockfd] = conn;

    // 设置关闭回调
    conn->set_close_callback([this](const std::shared_ptr<net::TcpConnection>& conn) {
        main_loop_->run_in_loop([this, conn]() {
            connections_.erase(conn->get_fd());
            conn->get_event_loop()->queue_in_loop([conn]() { conn->connect_destroyed(); });
        });
    });
    // 建立连接
    conn->connect_established();
}

}  // namespace lean_web_server::http
