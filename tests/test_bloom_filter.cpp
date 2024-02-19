//
// Created by bokket on 2024/2/18.
//
#include <memory>
#include <gtest/gtest.h>

#include "../Log/Log.h"
#include "../bloomfilter/bloom_filter.h"

using namespace bokket;
using namespace std;

TEST(bloom_filter, basic) {
    std::vector<std::string> data;
    for(int i=0;i<10*10000;++i) {
        data.emplace_back("key_"+ to_string(i));
    }
    std::unique_ptr<FilterPolicy> filterPolicy=std::make_unique<BloomFilter>(10 * 10000, 0.01);

    filterPolicy->CreateFilter(data);

    int cnt=0;
    for(int i=0;i<10*10000;++i) {
        if(filterPolicy->exists("key_"+std::to_string(i))) {
            ++cnt;
        }
    }

    EXPECT_EQ(10 * 10000, cnt);

    LOG_INFO("{}",cnt);
    cnt = 0;
    for (int i = 10 * 10000; i < 20 * 10000; ++i) {
        if (filterPolicy->exists("key_" + std::to_string(i))) {
            ++cnt;
        }
    }
    LOG_INFO("{}",cnt);
    EXPECT_LT(cnt, 3000); // cnt预期是1000（10条数据，假阳性概率为1%，预期为1000）
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
