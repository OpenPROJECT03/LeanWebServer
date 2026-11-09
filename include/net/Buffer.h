/**
 * @file Buffer.h
 * @brief 缓冲区类，用于管理读写数据
 * @author woruo03
 * @date 2026-10-30
 * @version 1.0
 */

#pragma once
#include <vector>

namespace lean_web_server::net {

// 缓冲区类，用于管理读写数据
// [TODO]: 实现定期的内存自动缩容
class Buffer {
public:
    static constexpr size_t k_initial_size = 1024;

    Buffer(size_t initial_size = k_initial_size) : buffer_(initial_size) {}

    // 从文件描述符中读取数据
    int read_fd(int fd, int& saved_errno);
    // 追加数据到缓冲区
    void append(const char* data, size_t size);
    // 标记已读数据
    void mark_used(size_t size);
    [[nodiscard]] size_t read_available() const { return write_index_ - read_index_; }
    [[nodiscard]] size_t write_available() const { return buffer_.size() - write_index_; }
    [[nodiscard]] const char* read_ptr() const { return buffer_.data() + read_index_; }
    [[nodiscard]] const char* write_ptr() const { return buffer_.data() + write_index_; }

private:
    std::vector<char> buffer_;
    size_t read_index_ = 0;
    size_t write_index_ = 0;

    // 确保缓冲区有足够的写空间，如果不足则扩容或搬运数据
    void ensure_write_available(size_t size);
};

}  // namespace lean_web_server::net
