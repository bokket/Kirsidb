//
// Created by bokket on 2024/1/31.
//

#include <memory>
#include <thread>
#include <gtest/gtest.h>

#include "../Log/Log.h"

#include <fmt/format.h>

#include "../file/file_writer.h"

using namespace bokket;

TEST(file_writer,append) {
    auto path="/home/bokket/bokedb1/file/appendtests.sst";
    auto cmd=fmt::format("rm -rf {}",path);

    LOG_INFO("cmd:{}",cmd);
    std::system(cmd.data());

    auto writer=std::make_unique<Writer>(path);

    std::string data1 = "12345";

    writer->append("wxzcdetr");
    writer->append("1234567");

    writer->close();
}

TEST(file_writer,basic) {
    {
        auto path = "/home/bokket/bokedb1/file/tests.sst";
        auto cmd = fmt::format("rm -rf {}", path);

        LOG_INFO("cmd:{}", cmd);
        std::system(cmd.data());

        auto writer = std::make_unique<PosixFileWriter>(path);

        std::string data1 = "123gfds4h6.1 s0 3ds 4g00 x_";
        std::string data2 = "  7112gdf455 4 56 u 455u 123fgh12fgn  ";
        std::string data3 = ".*-123 34fe r65 4r1 0m1 j0.10,/,.";

        writer->append(data1);
        writer->append(data2);
        writer->append(data3);
        writer->append(data1);

        writer->close();
        //writer->sync();
    }

//    auto reader=std::make_unique<ReaderFile>("/home/bokket/bokedb1/file/tests.sst");
//
//    std::string s;
//    reader->read(0,2,&s);
//
//    LOG_INFO("{}",s);

  //  writer->close();
}


TEST(randomAccess,read) {
    auto reader=std::make_unique<ReaderFile>("/home/bokket/bokedb1/file/tests.sst");

    std::string s;
    reader->read(0,2,&s);

    LOG_INFO("{}",s.data());
//    LOG_INFO("{}",std::string_view{b,2});
}

//TEST(file_writer,multibasic) {
//    auto path="/home/bokket/bokedb1/file/multitests.sst";
//
//    auto cmd=fmt::format("rm -rf {}",path);
//    LOG_INFO("cmd:{}",cmd);
//    std::system(cmd.data());
//
//    std::vector<std::thread> thr;
//
//    //auto writer=std::make_unique<FileWriter>(path);
//    auto writer=std::make_unique<Writer>(path);
//
//    std::string_view data1 = "123";
//    std::string_view data2 = "  7112gdf455 4 56 u 455u 123fgh12fgn  ";
//    std::string_view data3 = ".*-123 34fe r65 4r1 0m1 j0.10,/,.";
//
//    std::mutex mutex;
//
//    for(auto i=0;i<10;++i) {
//        thr.emplace_back([&writer,&data1,&data2,&mutex] {
//
//            std::thread t([&writer, &data1,&data2,&mutex]{
//               std::unique_lock<std::mutex> uniqueLock{mutex};
//
//                writer->append(data1);
//
//                //writer->sync();
//
//                writer->close();
//                //writer->append(data2);
////                writer->append(data2.data(),static_cast<int>(data2.size()));
////                writer->append(data3.data(),static_cast<int>(data3.size()));
////                writer->append(data1.data(),static_cast<int>(data1.size()));
//
//
//                //writer->sync();
//            });
//
//            t.detach();
//        });
//    }
//
//   // writer->close();
//
//    for(auto &t:thr) {
//        if (t.joinable()) {
//            t.join();
//        }
//    }
//}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}