//
// Created by bokket on 24-4-9.
//
#include <gtest/gtest.h>

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
       // LOG_INFO("{}",count);
        //fmt::printf(count);
        //PRINT_INFO("%d",count);
    }
}

TEST(log,basic) {
    Logger::getInstance()->init("../logs/test.log");

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
    std::mutex mutex;

    for(int i=0;i<50;++i) {
        thr.emplace_back([i,&mutex] {
            std::unique_lock<std::mutex> uniqueLock{mutex};
            Stdout(i);
        });
    }

    for(auto &t:thr) {
        if (t.joinable())
            t.join();
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}