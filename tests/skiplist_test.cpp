#include <gtest/gtest.h>

#include "../memtable/skiplist.h"
#include "../memtable/skiplist.cpp"

using namespace bokket;
using namespace std;

using Key=uint64_t;

struct Comparator {
  int operator()(const Key& a, const Key& b) const {
    if (a < b) {
      return -1;
    } else if (a > b) {
      return +1;
    } else {
      return 0;
    }
  }
};

std::mt19937 random_gen;

inline int RandomIn(int a, int b) {
    std::uniform_int_distribution<int> dis(a, b);
    return dis(random_gen);
}

TEST(SkipListTest,Empty)
{
    Comparator cmp;
    auto list=new SkipList<Key,Comparator>(cmp);
    EXPECT_TRUE(!list->Contains(10));


    auto iter=new SkipList<Key, Comparator>::Iter(list);

    EXPECT_TRUE(!iter->Valid());

    
    auto begin=list->Begin();
    EXPECT_EQ(begin.Valid(), false);

    auto it=list->Find(100);

    EXPECT_EQ(it.Valid(), false);

    auto end=list->End();

    EXPECT_EQ(end.Valid(), false);

    list->Print();

    delete list;
    delete iter;
}

TEST(SkipListTest, Insert)
{
   const int N = 2000;
   const int R = 5000;

   std::set<Key> keys;

   Comparator cmp;
   auto list=new SkipList<Key,Comparator>(cmp);


   for(int i=0;i<N;++i)
   {
      Key key=RandomIn(0, 1 << 4);
    
      if(keys.insert(key).second)
        list->Insert(key);
   }

   list->Print();

   for(int i=0;i<R;++i) {
    if(list->Contains(i))
    {
      EXPECT_EQ(keys.count(i), 1);
    } else {
      EXPECT_EQ(keys.count(i), 0);
    }
   }
   delete list;
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}