//
// Created by bokket on 24-4-12.
//

#include <gtest/gtest.h>


#include "./random.h"
#include "./FileSink.h"

#include "../sstable/Options.h"
#include "../sstable/SSTable.h"
#include "../sstable/SSTable_builder.h"

#include "../file/file_writer.h"

using namespace bokket;
using namespace std;

// TEST(sstable,basic) {
//    kvMap table;
//    Options options;

//    FileSink sink;

//     SSTableBuilder builder(&options,&sink);

//    for(int num_entries=1;num_entries<2000;num_entries += (num_entries < 50) ? 1 : 200)
//    {

//        LOG_INFO("entries:{}",num_entries);

//        for (int i = 0; i < num_entries; ++i) {
//            auto key = RandomString(RandomIn(0, 1 << 4));
//            auto value = RandomString(RandomIn(0, 1 << 5));
//            table.emplace(std::make_pair(std::move(key), std::move(value)));
//        }

//        for (const auto& it : table) {
//            builder.Add(it.first, it.second);
//        }

//        LOG_INFO("{}",sink.Content().size());
   
//         builder.Finish();
//    }

// }

// TEST(sstable,basic_self)
// {

//     auto path="/home/bokket/bokedb1/tests/basic_self.txt";

//     auto cmd=fmt::format("rm -rf {}",path);
//     LOG_INFO("cmd:{}",cmd);
//     std::system(cmd.data());

//    Options options;
//    FileWriter fileWriter("/home/bokket/bokedb1/tests/basic_self.txt");
//    SSTableBuilder builder(&options,&fileWriter);

//    builder.Add("key_0", "value_0");
//    builder.Add("key_0", "value_0");
//    builder.Add("key_0", "value_0");

//    builder.Finish();

//     builder.Add("key_1", "value_0");

// }

TEST(ReadBlockFromFile,read) {
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
    builder.Add("key_10", "value_0");
    builder.Add("key_11", "value_0");
    builder.Add("key_12", "value_0");

    builder.Finish();


    auto size=FileTool::GetFileSize("/home/bokket/bokedb1/tests/sst.txt");
    LOG_INFO("{}",size);

    //FileSource source(sink.Content());
    //std::unique_ptr<SSTable> sst(SSTable::Open(&readerFile,size));
    auto readerFile=new ReaderFile("/home/bokket/bokedb1/tests/sst.txt");
    std::unique_ptr<SSTable> sst(SSTable::Open(readerFile,size));
    //std::unique_ptr<SSTable> sst= make_unique<SSTable>(&options,&readerFile);
    //SSTable sst(&options,&readerFile,size);
    //SSTable* sst;

    //SSTable::Open(readerFile, &sst, size);
    //auto s=SSTable::Open(&readerFile,size,&sst);

    LOG_INFO("{}",sst->begin().Key());

    auto end=sst->end();

    auto begin=sst->begin();

    begin++;

    begin++;

    LOG_INFO("{},{}",begin.Key(),begin.Value());

    begin++;

    //EXPECT_EQ(begin,end);

    
    auto it=sst->begin();
    for(;it!=sst->end();it++)
    {
        LOG_INFO("{},{}",it.Key(),it.Value());
    }

    EXPECT_EQ(it, end);

    for(auto r=sst->find("key_0");r!=sst->end();r++)
    {

        LOG_INFO("{},{}",r.Key(),r.Value());
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


