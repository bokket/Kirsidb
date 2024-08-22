#include <gtest/gtest.h>
#include <string_view>

#include "../wal/wal_reader.h"
#include "../wal/wal_writer.h"

using namespace bokket;

TEST(WalTest,ReadWrite)
{
    
    auto path="/home/bokket/bokedb1/wal/log_test.wal";
    auto cmd=std::format("rm -rf {}",path);

    {
    LOG_INFO("cmd:{}",cmd);
    std::system(cmd.data());
    wal::Writer writer;
    writer.Open(path, tinyio::access_mode::write);
    writer.AppendRecordToBuffer("");
    writer.AppendRecordToBuffer("test");
    writer.AppendRecordToBuffer(std::string(1000,'-'));
    writer.Flush();
    }
    
    wal::Reader reader;
    reader.Open("../wal/log_test.wal");
    auto res=reader.ReadRecord();
    EXPECT_EQ(res.has_value(), true);
    EXPECT_EQ(*res,"");

    res=reader.ReadRecord();
    EXPECT_EQ(res.has_value(), true);
    EXPECT_EQ(*res,"test");

    res=reader.ReadRecord();
    EXPECT_EQ(res.has_value(), true);
    EXPECT_EQ(*res, std::string(1000,'-'));
    EXPECT_EQ(res->size(), 1000);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

