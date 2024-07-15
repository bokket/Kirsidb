//
// Created by bokket on 24-4-12.
//

#include <gtest/gtest.h>
#include <string_view>


#include "./random.h"
#include "./FileSink.h"

#include "../sstable/Options.h"
#include "../sstable/SSTable.h"
#include "../sstable/SSTable_builder.h"

#include "../file/file_writer.h"

using namespace bokket;
using namespace std;

// TEST(ReadBlockFromFile,read) {
//     auto path="/home/bokket/bokedb1/tests/sst.txt";

//     auto cmd=fmt::format("rm -rf {}",path);
//     LOG_INFO("cmd:{}",cmd);
//     std::system(cmd.data());

//     Options options;
//     PosixFileWriter fileWriter("/home/bokket/bokedb1/tests/sst.txt");
//     //FileSink sink;
//     SSTableBuilder builder(&options,&fileWriter);

//     builder.Add("key_0", "value_0");
//     builder.Add("key_1", "value_0");
//     builder.Add("key_2", "value_0");
//     builder.Add("key_3", "value_0");
//     builder.Add("key_4", "value_0");
//     builder.Add("key_5", "value_0");
//     builder.Add("key_6", "value_0");
//     builder.Add("key_7", "value_0");
//     builder.Add("key_8", "value_0");
//     builder.Add("key_9", "value_0");
//     builder.Add("key_10", "value_0");
//     builder.Add("key_11", "value_0");
//     builder.Add("key_12", "value_0");

//     builder.Finish();

//     //FileSource source(sink.Content());
//     auto readerFile=new ReaderFile("/home/bokket/bokedb1/tests/sst.txt");
//     std::unique_ptr<SSTable> sst(SSTable::Open(readerFile,builder.fileSize()));

//     LOG_INFO("{}",sst->begin().Key());

//     auto end=sst->end();

//     auto begin=sst->begin();

//     begin++;

//     begin++;

//     LOG_INFO("{},{}",begin.Key(),begin.Value());
    
//     auto it=sst->begin();
//     for(;it!=sst->end();it++)
//     {
//         LOG_INFO("{},{}",it.Key(),it.Value());
//     }

//     EXPECT_EQ(it, end);


//     // std::string res;
//     // auto r=sst->find("key_6",res);

//     // LOG_INFO("{}",res);

//     for(auto r=sst->find("key_7");r!=sst->end();r++)
//     {
//         LOG_INFO("{},{}",r.Key(),r.Value());
//     }
// }

TEST(ReadBlockFromFile,seek)
{
    auto path="/home/bokket/bokedb1/tests/sst.txt";

    auto cmd=fmt::format("rm -rf {}",path);
    LOG_INFO("cmd:{}",cmd);
    std::system(cmd.data());

    Options options;
    PosixFileWriter fileWriter("/home/bokket/bokedb1/tests/sst.txt");
    //FileSink sink;
    SSTableBuilder builder(&options,&fileWriter);

    builder.Add("key_0", "value_0");
    builder.Add("key_1", "value_0");
    builder.Add("key_2", "value_0");
    builder.Add("key_3", "value_0");
    builder.Add("key_4", "value_0");
    builder.Add("key_5", "value_0");
    builder.Add("key_6", "value_0");
    builder.Add("key_7", "value_0");
    builder.Add("key_8", "value_0");
    builder.Add("key_9", "value_0");
    builder.Add("key_10", "value_999");
    builder.Add("key_11", "value_0");
    builder.Add("key_12", "value_0");

    builder.Finish();

    //FileSource source(sink.Content());
    auto readerFile=new ReaderFile("/home/bokket/bokedb1/tests/sst.txt");
    std::unique_ptr<SSTable> sst(SSTable::Open(readerFile,builder.fileSize()));

    auto find=sst->find("key_10");
    LOG_INFO("{},{}",find.Key(),find.Value());

    // find=sst->find("key_6");
    // LOG_INFO("{},{}",find.Key(),find.Value());
    // std::string res;
    // auto r=sst->find("key_6",res);

    // LOG_INFO("{}",res);
} 

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}