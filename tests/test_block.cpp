//
// Created by bokket on 2024/2/7.
//
#include <memory>
#include <gtest/gtest.h>

#include "../sstable/block.h"

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

    auto reader=std::make_unique<Block>();
    reader->Init(result);

    auto value=reader->Get("key_1");
    for(auto it=value.begin();it!=value.end();it++) {
        LOG_INFO("{}",it->data());
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
