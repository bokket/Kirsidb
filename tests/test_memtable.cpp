#include <gtest/gtest.h>

#include "../sstable/Buf.h"

#include "../memtable/memtable.h"

using namespace bokket;
using namespace std;

TEST(Basic, empty)
{
    InternalKeyComparator cmp(newByteComparator());
    Memtable table(cmp);
    EXPECT_EQ(table.begin(), table.end());
}

TEST(Basic, Add)
{
    InternalKeyComparator cmp(newByteComparator());
    Memtable table(cmp);

    table.Add(1, ValueType::kTypePut, Buf("abc"), Buf("def"));

    LookupKey look("abc",1);

    LOG_INFO("{}",look.memtable_key().data());
    


    std::string s;
    auto b=table.find(look, &s);


    EXPECT_TRUE(b);
    EXPECT_EQ(s, "def");
    LOG_INFO("{}",s);

    table.Add(1, bokket::ValueType::kTypeDelete, Buf("abc"), Buf(""));
    LookupKey look1("abc",1);

    std::string s1;
    auto c=table.find(look1, &s1);
    LOG_INFO("{}",c);
    LOG_INFO("{}",s1);
}

TEST(Basic, iter)
{
    InternalKeyComparator cmp(newByteComparator());
    Memtable table(cmp);

    table.Add(1, ValueType::kTypePut, Buf("abc"), Buf("def"));

    auto iter=table.NewIterator();
    iter->SeekToFrist();

    LOG_INFO("{} | {}",iter->key(),iter->value());

    ParsedInternalKey ikey;

    Parsed(iter->key(), &ikey);

    LOG_INFO("{} | {} | {}",ikey.seq_,static_cast<int>(ikey.type_),ikey.user_key_);
    
    LOG_INFO("{}",ikey.DebugString());

    EXPECT_EQ(ikey.seq_, 1);
    EXPECT_EQ(ikey.type_, ValueType::kTypePut);
    EXPECT_EQ(ikey.user_key_, "abc");

    delete iter;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}