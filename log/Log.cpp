//
// Created by bokket on 2023/12/5.
//

#include "Log.h"

using namespace bokket;

Logger::~Logger() {
    // if (isInited_) {

    //     shutdown();
    // }
}



//bool Logger::init(std::string_view filePath, std::string_view logName, spdlog::level::level_enum level)
bool Logger::init(std::string_view filePath, spdlog::level::level_enum level){
    if(isInited_)
        return true;
    try {
        fs::path logPath(filePath);
        fs::path logDir = logPath.parent_path();
        if(!fs::exists(logPath)) {
            fs::create_directories(logDir);
        }

        constexpr std::size_t logBufferSize = 32* 1024 ;
        spdlog::init_thread_pool(logBufferSize,std::thread::hardware_concurrency());
        std::vector<spdlog::sink_ptr> sinks;

        //daily_logger_mt 线程安全
        //daily_logger_st not-mutex
        //https://www.cnblogs.com/fortunely/p/16870881.html#sinks%E7%BB%A7%E6%89%BF%E4%BD%93%E7%B3%BB
        auto dailySink= std::make_shared<spdlog::sinks::daily_file_sink_mt>(logPath.c_str(),12,00);
        sinks.emplace_back(dailySink);

        auto consoleSink= std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.emplace_back(consoleSink);

        auto logger=std::make_shared<spdlog::logger>("xx",sinks.begin(), sinks.end());

        logger->set_level(level);


        //default format is in the form of:
        //[2014-10-31 23:46:59.678] [my_loggername] [info] Some message

        //%H:%M:%S
        //24小时制的小时:十时制表示的分钟:十进制的秒数
        //format:yyyy-mm-dd hh:mm::ss
        //spdlog::set_pattern("%s(%#): [%L %D %T.%e %P %t %!] %v");
        //4位数年份-月份-月份的某一天

        //日志级别 tidID [源文件 : 源行 <源函数>要记录的实际文本]
        logger->set_pattern("%Y-%m-%d %H:%M:%S [%l] [tid : %t] [%s : %# <%!>] %v");

        spdlog::flush_on(spdlog::level::warn);

        //spdlog::set_default_logger(std::make_shared<spdlog::logger>("xx",sinks.begin(),sinks.end()));
        spdlog::set_default_logger(logger);
    }
    catch (const spdlog::spdlog_ex& ex) {
        //isInited_= false;
        std::cout << "Log initialization failed: " <<ex.what()<<std::endl;
    }
    isInited_= true;
    return true;
}

