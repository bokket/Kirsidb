#include <gtest/gtest.h>
#include <memory>

#include "../sstable/table_cache.h"
#include "../sstable/SSTable_builder.h"

using namespace bokket;


TEST(Basic,table_cache)
{
    auto path="/home/bokket/bokedb1/tests/000020.sst";

    auto cmd=fmt::format("rm -rf {}",path);
    LOG_INFO("cmd:{}",cmd);
    std::system(cmd.data());

    Options options;
    PosixFileWriter fileWriter("/home/bokket/bokedb1/tests/000020.sst");
    SSTableBuilder builder(&options,&fileWriter);

    builder.Add("key_0", "value_0");
    builder.Add("key_1", "value_0");
    builder.Add("key_2", "value_0");
    builder.Add("key_3", "value_0");
    builder.Add("key_4", "value_0");
    builder.Add("key_5", "value_0");
    builder.Add("key_6", "value_1");
    builder.Add("key_7", "value_0");
    builder.Add("key_8", "value_1");
    builder.Add("key_9", "value_2");
    builder.Add("key_10", "value_3");
    builder.Add("key_11", "value_4");
    builder.Add("key_12", "value_5");

    builder.Finish();


    auto table_cache=std::make_unique<TableCache>("/home/bokket/bokedb1/tests",10);
    LOG_INFO("{}",builder.fileSize());
    std::string result;
    auto s=table_cache->Get(20,   builder.fileSize() ,"key_12" , result);

    LOG_INFO("{}",result);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}