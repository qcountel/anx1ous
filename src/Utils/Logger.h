#pragma once

#include <cstdio>
#include <format>
#include <mutex>
#include <string>
#include <string_view>

namespace anx1ous {

enum class LogLevel { Trace, Debug, Info, Warn, Error };

class Logger {
public:
    static Logger& get();

    void init();
    void shutdown();

    void setLevel(LogLevel level) { m_level = level; }
    LogLevel level() const { return m_level; }

    void write(LogLevel level, std::string_view tag, std::string_view message);

private:
    Logger() = default;

    std::mutex m_mutex;
    FILE* m_file = nullptr;
    LogLevel m_level = LogLevel::Debug;
};

namespace detail {
template <typename... Args>
inline void logf(LogLevel level, std::string_view tag, std::format_string<Args...> fmt, Args&&... args) {
    if (level < Logger::get().level())
        return;
    Logger::get().write(level, tag, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
int sink(const Args&...);
}

}

#define LOG_ERROR(tag, ...) ::anx1ous::detail::logf(::anx1ous::LogLevel::Error, tag, __VA_ARGS__)

#if defined(ANX1OUS_LOGGING)
#define LOG_TRACE(tag, ...) ::anx1ous::detail::logf(::anx1ous::LogLevel::Trace, tag, __VA_ARGS__)
#define LOG_DEBUG(tag, ...) ::anx1ous::detail::logf(::anx1ous::LogLevel::Debug, tag, __VA_ARGS__)
#define LOG_INFO(tag, ...)  ::anx1ous::detail::logf(::anx1ous::LogLevel::Info,  tag, __VA_ARGS__)
#define LOG_WARN(tag, ...)  ::anx1ous::detail::logf(::anx1ous::LogLevel::Warn,  tag, __VA_ARGS__)
#else
#define LOG_TRACE(tag, ...) ((void)sizeof(::anx1ous::detail::sink(tag, __VA_ARGS__)))
#define LOG_DEBUG(tag, ...) ((void)sizeof(::anx1ous::detail::sink(tag, __VA_ARGS__)))
#define LOG_INFO(tag, ...)  ((void)sizeof(::anx1ous::detail::sink(tag, __VA_ARGS__)))
#define LOG_WARN(tag, ...)  ((void)sizeof(::anx1ous::detail::sink(tag, __VA_ARGS__)))
#endif
