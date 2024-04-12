//
// Created by bokket on 24-4-12.
//

#include <gtest/gtest.h>

#include "../file/file_writer.h"
#include "./random.h"
#include "./FileSink.h"

#include "../sstable/Options.h"
#include "../sstable/SSTable_builder.h"

using namespace bokket;
using namespace std;

//TEST(sstable,basic) {
//    kvMap table;
//    Options options;
//
//    FileSink sink;
//
//    for(int num_entries=1;num_entries<2000;num_entries += (num_entries < 50) ? 1 : 200)
//    {
//        SSTableBuilder builder(&options,&sink);
//
//        LOG_INFO("entries:{}",num_entries);
//
//        for (int i = 0; i < num_entries; ++i) {
//            auto key = RandomString(RandomIn(0, 1 << 4));
//            auto value = RandomString(RandomIn(0, 1 << 5));
//            table.emplace(std::make_pair(std::move(key), std::move(value)));
//        }
//
//        for (const auto& it : table) {
//            builder.Add(it.first, it.second);
//        }
//
//        LOG_INFO("{}",sink.Content().size());
//
//        builder.Finish();
//
//    }
//
//}

TEST(sstable,basic)
{
    Options options;
    FileWriter fileWriter("/home/bokket/bokedb1/tests/sst.txt");
    SSTableBuilder builder(&options,&fileWriter);

    builder.Add("key_0", "value_0");
    builder.Add("key_0", "value_0");
    builder.Add("key_0", "value_0");

    builder.Finish();

//    builder.Add("key_1", "value_0");

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


