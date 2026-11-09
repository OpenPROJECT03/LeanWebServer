/**
 * @file HttpContext.h
 * @brief HTTP 请求上下文
 * @author woruo03
 * @date 2026-11-01
 * @version 1.0
 */

#pragma once
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace lean_web_server::net {
class Buffer;
}

namespace lean_web_server::http {

class HttpContext {
public:
    enum class HttpRequestParseState : uint8_t {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kDone,
    };

    [[nodiscard]] HttpRequestParseState parse_state() const { return parse_state_; }
    [[nodiscard]] std::string_view method(const char* data) const { return method_.as_view(data); }
    [[nodiscard]] std::string_view path(const char* data) const { return path_.as_view(data); }
    [[nodiscard]] std::string_view version(const char* data) const {
        return version_.as_view(data);
    }
    [[nodiscard]] size_t checked_bytes() const { return checked_bytes_; }

    void reset() {
        parse_state_ = HttpRequestParseState::kExpectRequestLine;
        checked_bytes_ = 0;
        method_ = {};
        path_ = {};
        version_ = {};
        body_ = {};
        headers_.clear();
    }

    // 解析HTTP请求
    bool parse_request(net::Buffer& buffer);

private:
    struct StringSlice {
        size_t offset;
        size_t length;
        std::string_view as_view(const char* data) const {
            if ((data == nullptr) || (length == 0)) {
                return {};
            }
            return {data + offset, length};
        }
    };

    HttpRequestParseState parse_state_{};
    size_t checked_bytes_{};
    size_t body_length_{};

    StringSlice method_{};
    StringSlice path_{};
    StringSlice version_{};
    StringSlice body_{};
    std::vector<std::pair<StringSlice, StringSlice>> headers_;

    [[nodiscard]] const char* find_crlf(const net::Buffer& data) const;
    bool process_request_line(const char* begin, const char* end);
    bool process_headers(const char* begin, const char* end);
    static bool case_insensitive_equal(std::string_view a, std::string_view b) noexcept;
};

}  // namespace lean_web_server::http
