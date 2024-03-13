//
// Created by bokket on 2023/12/5.
//

#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <string>
#include <string_view>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <memory>
#include <functional>
#include <chrono>
#include <type_traits>
#include <iostream>
#include <filesystem>
#include <atomic>
#include <shared_mutex>

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/pattern_formatter.h>

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/printf.h>
//#include <fmt/format.h>
//#include <fmt/printf.h>
//#include <fmt/core.h>

#include "../base/noncopyable.h"

namespace bokket
{

namespace fs=std::filesystem;

class Logger {

public:
    struct LogStream:public std::ostringstream
    {
    public:
        LogStream(const spdlog::source_loc& loc,spdlog::level::level_enum lvl,std::string_view prefix)
                :loc_(loc)
                ,lvl_(lvl)
                ,prefix_(std::move(prefix)) {}

        ~LogStream() {
            flush();
        }

        void flush() {
            Logger::getInstance()->log(loc_,lvl_,(prefix_+ this->str().c_str()));
            std::cout<< this->str()<<std::endl;
            std::cout<< prefix_.data()<<std::endl;
        }

    private:
        spdlog::source_loc loc_;
        spdlog::level::level_enum lvl_=spdlog::level::info;
        std::string prefix_;
    };

public:
    //explicit Logger(std::string_view file,int32_t line,std::string_view funcName,LogLevel logLevel,std::string_view format, ...);
    bool init(std::string_view filePath, spdlog::level::level_enum level=spdlog::level::info);


public:
    Logger()=default;
    ~Logger();

    static Logger* getInstance() {
        static Logger logger;
        return &logger;
    }

    template<typename... Args>
    void log(const spdlog::source_loc& loc,spdlog::level::level_enum lvl,std::string_view fmt,const Args& ... args) {
        spdlog::log(loc,lvl,fmt,args...);
    }

    template<typename... Args>
    void print(const spdlog::source_loc& loc,spdlog::level::level_enum lvl,std::string_view fmt,const Args& ... args) {
        spdlog::log(loc,lvl,fmt::sprintf(fmt,args...).c_str());
    }

    void setLevel(spdlog::level::level_enum lvl) {
        level_=lvl;
        spdlog::set_level(lvl);
    }
    static const char* getFileName(std::string_view path) {
        if(path.empty())
            return path.data();
        std::size_t pos= path.find_last_of("/\\");
        return path.data()+((pos==path.npos)? 0 : pos+1);
    }
public:
    static void shutdown() {
        spdlog::shutdown();
    }
private:
    std::mutex mutex_;
    std::atomic<bool> isInited_{false};
    spdlog::level::level_enum level_=spdlog::level::trace;
};


}

#define __FILE_NAME__ (bokket::Logger::getFileName(__FILE__))

//#define LOG_INFO(fmt,...)   spdlog::log({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::trace,fmt,##__VA_ARGS__)

#define LOG_TRACE(fmt,...)   spdlog::log({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::trace,fmt,##__VA_ARGS__)
#define LOG_INFO(fmt,...)   spdlog::log({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::info,fmt,##__VA_ARGS__)
#define LOG_DEBUG(fmt,...)   spdlog::log({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::debug,fmt,##__VA_ARGS__)
#define LOG_WARN(fmt,...)   spdlog::log({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::warn,fmt,##__VA_ARGS__)
#define LOG_ERROR(fmt,...)   spdlog::log({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::err,fmt,##__VA_ARGS__)


#define PRINT_TRACE(fmt,...) bokket::Logger::getInstance()->print({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::trace,fmt,##__VA_ARGS__)
#define PRINT_INFO(fmt,...) bokket::Logger::getInstance()->print({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::info,fmt,##__VA_ARGS__)
#define PRINT_DEBUG(fmt,...) bokket::Logger::getInstance()->print({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::debug,fmt,##__VA_ARGS__)
#define PRINT_WARN(fmt,...) bokket::Logger::getInstance()->print({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::warn,fmt,##__VA_ARGS__)


#define STREAM_TRACE()        bokket::Logger::LogStream({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::trace,"")
#define STREAM_INFO()        bokket::Logger::LogStream({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::debug," ")
#define STREAM_DEBUG()        bokket::Logger::LogStream({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::debug," ")
#define STREAM_WARN()        bokket::Logger::LogStream({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::warn,"test")
#define STREAM_ERROR()        bokket::Logger::LogStream({__FILE_NAME__,__LINE__,__FUNCTION__},spdlog::level::err," ")
