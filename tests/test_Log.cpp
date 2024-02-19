//
// Created by bokket on 2023/12/8.
//

#include "../Log/Log.h"

using namespace bokket;

void test_log() {
    Logger::getInstance()->setLevel(spdlog::level::info);
    STREAM_WARN() << "STM_WARN " << 3;
    PRINT_WARN("PRINT_WARN, %d", 3);
    LOG_INFO("LOG_INFO {}", 3);
    STREAM_INFO()<<"1";
}

void Stdout(int count) {
    for(int32_t i=0;i<count;++i) {
        //LOG_INFO("{}",count);

        PRINT_INFO("%d",count);
    }
}


int main() {
    if (!Logger::getInstance()->init("../logs/test.log"))
    {
        return 0;
    }

    Logger::getInstance()->setLevel(spdlog::level::trace);



    STREAM_DEBUG() << "STM_DEBUG " << 1;
    PRINT_WARN("PRINT_WARN, %d", 1);
    LOG_INFO("LOG_INFO {}", 1);

    LOG_TRACE("LOG_TRACE ZYK {}","zyk");

    Logger::getInstance()->setLevel(spdlog::level::info);
    STREAM_DEBUG()<<"STM_DEBUG " << 2;
    PRINT_WARN("PRINT_WARN, %d", 2);
    LOG_INFO("LOG_INFO {}", 2);

    test_log();


    STREAM_ERROR()<<"error test";

    auto n=-1;
    LOG_ERROR("Allocate::New(n) n <=0 n={}",n);

    // call before spdlog static variables destroy
    //Logger::getInstance()->shutdown();

    std::vector<std::thread> thr;

    for(int i=0;i<50;++i) {
        thr.emplace_back([i] {
            std::thread t(Stdout, i);
            t.detach();
        });
    }

    return 0;
}