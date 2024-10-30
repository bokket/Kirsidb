//
// Created by bokket on 2023/12/5.
//

#pragma once

#define SPDLOG_USE_STD_FORMAT


#include <string>
#include <atomic>
#include <format>
#include <string_view>
#include <iostream>
#include <filesystem>
#include <source_location>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/common.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/pattern_formatter.h>

namespace bokket
{

namespace fs=std::filesystem;

[[nodiscard]] constexpr auto get_log_source_location(
    const std::source_location &location) {
  return spdlog::source_loc{location.file_name(),
                            static_cast<std::int32_t>(location.line()),
                            location.function_name()};
}

struct format_with_location {
    //const std::format_string<Args...> fmt_;
    std::string_view fmt_;
    spdlog::source_loc loc_;

    template <typename Fmt>
    explicit format_with_location(
        const Fmt &fmt,
        const std::source_location &location = std::source_location::current())
        : fmt_{fmt}, loc_{get_log_source_location(location)} {}
};


class Logger {
public:
    struct LogStream:public std::ostringstream
    {
    public:
        LogStream(const std::source_location& loc,spdlog::level::level_enum lvl,std::string_view prefix)
                :loc_{loc}
                ,lvl_{lvl}
                ,prefix_{prefix} {}

        ~LogStream() {
            flush();
        }
        void flush() {
            Logger::getInstance()->log(format_with_location((prefix_+ this->str()),loc_),lvl_);
        }

    private:
        std::source_location loc_;
        spdlog::level::level_enum lvl_=spdlog::level::info;
        std::string prefix_;
    };

public:
    bool init(std::string_view filePath, spdlog::level::level_enum level=spdlog::level::info);


public:
    Logger()=default;
    ~Logger();

    static Logger& get() {
        static Logger logger;
        return logger;
    }

    static Logger* getInstance() {
        static Logger logger;
        return &logger;
    }

    // Ref:
    // https://www.reddit.com/r/cpp_questions/comments/1aohxk8/stdformat_error_call_to_consteval_function/
    template<typename... Args>
    static void log(format_with_location fmt,spdlog::level::level_enum lvl,Args&&... args) {
        spdlog::log(fmt.loc_,lvl,std::vformat(fmt.fmt_, std::make_format_args(std::forward<Args>(args)...)));
        // spdlog::log(fmt.loc_,lvl,std::format(fmt.fmt_,std::forward<Args>(args)...));
    }

    void setLevel(spdlog::level::level_enum lvl) {
        level_=lvl;
        spdlog::set_level(lvl);
    }

public:
    static void shutdown() {
        spdlog::shutdown();
    }
private:
    std::atomic<bool> isInited_{false};
    spdlog::level::level_enum level_=spdlog::level::trace;
};

}

namespace bokket
{

// Ref https://www.reddit.com/r/cpp_questions/comments/f7wf11/clang_va_opt_warning/?rdt=38997
// std https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1042r1.html
#define LOG_INFO(fmt, ...) bokket::Logger::getInstance()->log(bokket::format_with_location(fmt,std::source_location::current()),spdlog::level::info __VA_OPT__(,) __VA_ARGS__);
#define LOG_DEBUG(fmt, ...) bokket::Logger::getInstance()->log(bokket::format_with_location(fmt,std::source_location::current()),spdlog::level::debug __VA_OPT__(,) __VA_ARGS__);
#define LOG_WARN(fmt, ...) bokket::Logger::getInstance()->log(bokket::format_with_location(fmt,std::source_location::current()),spdlog::level::warn __VA_OPT__(,) __VA_ARGS__);
#define LOG_ERROR(fmt, ...) bokket::Logger::getInstance()->log(bokket::format_with_location(fmt,std::source_location::current()),spdlog::level::err __VA_OPT__(,) __VA_ARGS__);
#define LOG_TRACE(fmt, ...) bokket::Logger::getInstance()->log(bokket::format_with_location(fmt,std::source_location::current()),spdlog::level::trace __VA_OPT__(,) __VA_ARGS__);

#define PRINT_TRACE(fmt,...) Logger::getInstance()->log(format_with_location(fmt,std::source_location::current()),spdlog::level::trace __VA_OPT__(,) __VA_ARGS__)
#define PRINT_INFO(fmt,...) Logger::getInstance()->log(format_with_location(fmt,std::source_location::current()),spdlog::level::info __VA_OPT__(,) __VA_ARGS__)
#define PRINT_DEBUG(fmt,...) Logger::getInstance()->log(format_with_location(fmt,std::source_location::current()),spdlog::level::debug __VA_OPT__(,) __VA_ARGS__)
#define PRINT_WARN(fmt,...) Logger::getInstance()->log(format_with_location(fmt,std::source_location::current()),spdlog::level::warn __VA_OPT__(,) __VA_ARGS__)

#define STREAM_TRACE()        Logger::LogStream(std::source_location::current(),spdlog::level::trace,"")
#define STREAM_INFO()        Logger::LogStream(std::source_location::current(),spdlog::level::info,"")
#define STREAM_DEBUG()        Logger::LogStream(std::source_location::current(),spdlog::level::debug,"")
#define STREAM_WARN()        Logger::LogStream(std::source_location::current(),spdlog::level::warn,"")
#define STREAM_ERROR()        Logger::LogStream(std::source_location::current(),spdlog::level::err," ")

}