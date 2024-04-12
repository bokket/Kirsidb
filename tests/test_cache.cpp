//
// Created by bokket on 24-3-28.
//

#include <gtest/gtest.h>

#include "../cache/cache.h"

using namespace bokket;
using namespace std;

TEST(cache,init)
{
    auto lru=std::make_unique<LRUCachePolicy>(2);
//    std::unique_ptr<LRUCachePolicy> s(new LRUCachePolicy(5));
//    std::unique_ptr<LRUCachePolicy> s(LRUCachePolicy::Default(3));
    lru->Insert("3",1);

    auto handle=lru->Lookup("5");
    EXPECT_EQ(handle, nullptr);

    handle=lru->Insert("3",1);
    //LRUCachePolicy::LRUHandle h;
    auto v=std::any_cast<int>(lru->Value(handle));
    EXPECT_EQ(v,1);

    handle=lru->Lookup("3");
    v=std::any_cast<int>(lru->Value(handle));
    EXPECT_EQ(v,1);

    lru->Insert("2",1);

    lru->Insert("1",1);

    auto h=lru->Insert("5","q");
    LOG_INFO("{}",std::any_cast<decltype("q")>(h->value_));


//    LOG_INFO("{}",lru->Value(h).type().name());
//    auto s=std::any_cast<decltype(6.000)>(lru->Value(h));
//    LOG_INFO("{}",s);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}