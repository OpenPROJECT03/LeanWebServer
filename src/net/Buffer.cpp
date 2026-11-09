#include "net/Buffer.h"

#include <sys/uio.h>

#include <array>
#include <cerrno>
#include <cstring>

namespace lean_web_server::net {

int Buffer::read_fd(int fd, int& saved_errno) {
    size_t write_available_before = write_available();

    // 使用栈内存作为临时缓冲区，避免堆分配和锁竞争
    std::array<char, 65536> temp;
    std::array<struct iovec, 2> iov;

    iov[0].iov_base = buffer_.data() + write_index_;
    iov[0].iov_len = write_available_before;
    iov[1].iov_base = temp.data();
    iov[1].iov_len = temp.size();

    const int iov_cnt = write_available() < temp.size() ? 2 : 1;
    ssize_t n = ::readv(fd, iov.data(), iov_cnt);
    if (n < 0) {
        saved_errno = errno;
        return -1;
    }

    if (static_cast<size_t>(n) <= write_available_before) {
        write_index_ += static_cast<size_t>(n);
    } else {
        write_index_ = buffer_.size();
        append(temp.data(), static_cast<size_t>(n) - write_available_before);
    }
    return static_cast<int>(n);
}

void Buffer::append(const char* data, size_t size) {
    ensure_write_available(size);
    std::memcpy(buffer_.data() + write_index_, data, size);  // 将数据复制到缓冲区
    write_index_ += size;
}

void Buffer::ensure_write_available(size_t size) {
    if (write_available() < size) {
        if (write_available() + read_index_ < size) {  // 当缓冲区不足，需要扩容
            buffer_.resize(write_index_ + size);
        } else if (read_index_ > 0) {             // 当发生数据偏移才搬运
            size_t read_able = read_available();  // 可读数据量
            std::memmove(buffer_.data(), buffer_.data() + read_index_, read_able);  // 搬运数据
            read_index_ = 0;
            write_index_ = read_able;
        }
    }
}

void Buffer::mark_used(size_t size) {
    if (size < read_available()) {
        read_index_ += size;
    } else {
        read_index_ = write_index_ = 0;
    }
}

}  // namespace lean_web_server::net
