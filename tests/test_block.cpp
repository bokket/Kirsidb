//
// Created by bokket on 2024/2/7.
//
#include <memory>
#include <gtest/gtest.h>

#include "../sstable/block_iter.h"

using namespace bokket;
using namespace std;

TEST(block, init)
{
    auto builder=std::make_unique<BlockBuilder>();
    builder->add("key_0","value_0");
    builder->add("key_1","value_1");
    builder->add("key_2","value_2");
    builder->add("key_2","value_3");
    builder->add("key_1","value_2");
    builder->add("key_1","value_2");
    builder->add("key_1","value_2");

    LOG_INFO("{}",builder->data());

    std::string result;
    builder->final(result);
    LOG_INFO("{}",result);

    auto reader=std::make_unique<Block>(result);
    auto it=reader->begin();
    auto end=reader->end();

    auto k=it.key();
    auto v=it.value();

    LOG_INFO("{},{}",k,v);

    for(auto it=reader->begin();it!=reader->end();it++) {
        LOG_INFO("{},{}",it.key(),it.value());
    }

    for(auto it=reader->begin();it!=reader->end();it++) {
        auto r = reader->find("key_0");
        LOG_INFO("{},{}", r.key(), r.value());
    }

    EXPECT_EQ(it,reader->end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
