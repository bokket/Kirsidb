//
// Created by bokket on 2024/2/7.
//
#include <memory>
#include <map>
#include <gtest/gtest.h>

#include "random.h"
#include "../SkipList/comparator.h"
#include "../sstable/block_iter.h"

using namespace bokket;
using namespace std;

//struct newByteCmp {
//    const Comparator* cmp;
//    newByteCmp()
//            :cmp(newByteComparator())
//    {}
//    bool operator()(std::string_view  a,std::string_view b) const {
//        return cmp->Compare(a,b)<0;
//    }
//};
//
//using kvMap=std::map<std::string,std::string,newByteCmp>;

// TEST(block, init)
// {
//    auto builder=std::make_unique<BlockBuilder>();
//    builder->add("1ey_1","value_0");
//    builder->add("1ey_1","value_0");
//    builder->add("1ey_1","value_2");
//    builder->add("1key_1","value_2");
// //    builder->add("key_2","value_3");
// //    builder->add("key_1","value_2");
// //    builder->add("key_1","value_2");
// //    builder->add("key_1","value_2");

//    LOG_INFO("{}",builder->data());

//    std::string result;
//    builder->final(result);
//    LOG_INFO("{}",result);

//    auto reader=std::make_unique<Block>(result);
//    auto it=reader->begin();

// //    it++;
// //    it++;

// //    auto k=it.key();
// //    auto v=it.value();
// //
// //    LOG_INFO("{},{}",k,v);
// //    it++;
// //
// //    LOG_INFO("{},{}",it.key(),it.value());
// //
// //    it++;
// //
// //    LOG_INFO("{},{}",it.key(),it.value());
// //
// //    it++;
// //
// //    LOG_INFO("{},{}",it.key(),it.value());
// //
// //    it++;
// //
// //    auto end=reader->end();

// //    assert(end==it);

// //    auto end_k=end.key();
// //    auto end_v=end.value();

//    //LOG_INFO("{},{}",end_k,end_v);



//    for(;it!=reader->end();it++) {
//        LOG_INFO("{},{}",it.key(),it.value());
//    }

// //    EXPECT_EQ(it,end);
// //    for(it;it!=reader->end();it++) {
// //        auto r=reader->find("1key_1");
// //        LOG_INFO("{},{}", r.key(), r.value());
// //    }
//    //auto r=reader->find("2ey_1");

//    //LOG_INFO("{},{}", r.key(), r.value());

// //    for(auto it=reader->begin();it!=reader->end();it++) {
// //        //auto r = reader->find("key_0");
// //        LOG_INFO("{},{}", it.key(), it.value());
// //    }
// //
// //    EXPECT_EQ(it,reader->end());
// }

//TEST(basic,build)
//{
//    kvMap table;
//
//    //auto builder=std::make_unique<BlockBuilder>();
//    BlockBuilder builder;
//    int j=1,k=1;
//    for(int num=0;num<2000;num+=(num<50)? 1: 200) {
//        table.clear();
//        builder.clear();
//
//        LOG_INFO("num={}",num);
//
//
//        for(int i=0;i<num;i++) {
//            auto key= RandomString(RandomIn(0,1<<4));
//            auto value = RandomString(RandomIn(0, 1 << 5));
//            table.emplace(std::make_pair(std::move(key), std::move(value)));
//
//            //LOG_INFO("{} | {}",key,value);
//        }
//
//
//        for(const auto& it:table) {
////            LOG_INFO("{} | {}",it.first,it.second);
//            builder.add(it.first, it.second);
//        }
//
//        std::string result;
//        builder.final(result);
//       // LOG_INFO("{}",result);
//
//        Block block(result);
//        auto it=block.begin();
//
//        for(auto it2=table.begin();it2!=table.end();it2++,it++) {
//
//            ASSERT_NE(block.end(),it);
//
//            ASSERT_EQ(it.key(),it2->first);
//            ASSERT_EQ(it.value(),it2->second);
//
//            LOG_INFO("{}) {}\t\t{}",j++,it.key(),it.value());
//        }
//
//        ASSERT_EQ(block.end(),it);
//
////        for(auto it2=table.rbegin();it2!=table.rend();it2++)
////        {
////            if(RandomIn(0,1)==0) {
////                ASSERT_EQ(block.find(it2->first).key(),it2->first);
////                ASSERT_EQ(block.find(it2->first).value(),it2->second);
////
////                LOG_INFO("{}) {}\t\t{}",k++,block.find(it2->first).key(),block.find(it2->first).value());
////            }
////        }
//
//    }
////    for(const auto& it:table) {
////        builder.add(it.first, it.second);
////    }
////
////    std::string result;
////    builder.final(result);
////    // LOG_INFO("{}",result);
////
////    Block block(result);
////    auto it=block.begin();
////
////    for(auto it2=table.rbegin();it2!=table.rend();it2++) {
////        ASSERT_EQ(block.find(it2->first).key(),it2->first);
////        ASSERT_EQ(block.find(it2->first).value(),it2->second);
////
////        LOG_INFO("{}) {}\t\t{}",k++,block.find(it2->first).key(),block.find(it2->first).value());
////    }
//}

TEST(seek,build) {
        auto builder=std::make_unique<BlockBuilder>();
        //BlockBuilder builder;
        builder->add("key_2","value_3");
        builder->add("key_3","value_2");
        builder->add("key_4","value_4");
        builder->add("key_5","value_5");

        std::string result;
        builder->final(result);
        LOG_INFO("{}",result);

        auto reader=std::make_unique<Block>(result);
        //auto reader=std::make_unique<Block>();
        //auto s=reader->Init(result);
//        auto end=reader->end();
//
//        LOG_INFO("{},{}", end.key(), end.value());

        auto it=reader->begin();
    

        auto r = reader->find("key_2");
        LOG_INFO("{},{}", r.key(), r.value());

        r=reader->find("key_3");
        LOG_INFO("{},{}", r.key(), r.value());

        for(r=reader->find("key_3");r!=reader->end();r++)
        {
            LOG_INFO("{},{}",r.key(),r.value());
        }
        EXPECT_EQ(r,reader->end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
