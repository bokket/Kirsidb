//
// Created by bokket on 24-4-17.
//
#include <gtest/gtest.h>

#include "../file/file_writer.h"
#include "../sstable/Options.h"
#include "../sstable/SSTable_builder.h"

using namespace bokket;
using namespace std;

TEST(ReadBlockFromFile,read) {
    auto path="/home/bokket/bokedb1/tests/sst.txt";

    auto cmd=fmt::format("rm -rf {}",path);
    LOG_INFO("cmd:{}",cmd);
    std::system(cmd.data());

    Options options;
    Writer fileWriter("/home/bokket/bokedb1/tests/sst.txt");
    //FileSink sink;
    SSTableBuilder builder(&options,&fileWriter);

    builder.Add("key_0", "value_0");
    builder.Add("key_1", "value_0");
    builder.Add("key_2", "value_0");

    builder.Finish();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}