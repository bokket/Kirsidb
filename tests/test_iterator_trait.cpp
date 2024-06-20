//
// Created by bokket on 2024/2/21.
//
#include <memory>
#include <thread>
#include <gtest/gtest.h>

#include "../iterator/iterator_trait.h"

using namespace bokket;

//class TestIter {
//public:
//    class Iter;
//
//    Iter Begin() const;
//    Iter End() const;
//private:
//    int src_[2]={0,1};
//};
//
//class TestIter::Iter:public  IteratorFacade<Iter,const int,ForwardIteratorTag>
//{
//    friend class TestIter;
//
//public:
//    explicit Iter(const int* p)
//                 :p_(p) {}
//
//    //friend class IteratorCoreAccess;
//    void increment() {
//        p_++;
//    }
//
//    int const& dereference() const
//    {
//        return *p_;
//    }
//
//    bool equal(TestIter::Iter const& rhs) const
//    {
//        return rhs.p_ == p_;
//    }
//private:
//    int const* p_;
//};
//
//inline TestIter::Iter TestIter::Begin() const {
//    return Iter(&src_[0]);
//}
//
//inline TestIter::Iter TestIter::End() const {
//    return Iter(&src_[1]);
//}
//
//TEST(iter_trait,basic) {
//    TestIter iter;
//    auto it=iter.Begin();
//    printf("%d",*it);
//    it++;
//    printf("%d",*it);
//    auto end=iter.End();
//    printf("%d",*end);
//    EXPECT_EQ(it,iter.End());
//}

class SimpleObject
{
public:
    SimpleObject()=default;
    SimpleObject(int v)
                :v_(v)
    {}
    int Value() const {
        return v_;
    }

private:
    int v_;
};


template<bool IsConst>
class TestIter2:public IteratorFacade<TestIter2<IsConst>,SimpleObject,ForwardIteratorTag,IsConst>
{
    using Facade=IteratorFacade<TestIter2<IsConst>,SimpleObject,ForwardIteratorTag,IsConst>;
    using Reference=typename Facade::reference;

public:
    explicit TestIter2(const SimpleObject* p)
                     :p_(p)
    {}


    void increment() {
        p_++;
    }
    bool equal(const TestIter2 &other) const {
        return p_ == other.p_;
    }


    Reference dereference() const {
        return *p_;
    }

private:
    const SimpleObject* p_;
};

class TestArray
{
public:
    using ConstIterator=TestIter2<true>;
    using Iterator=TestIter2<false>;

    TestArray() {
        for (int i = 0; i < kArraySize; i++) {
            rep_[i] = i + 1;
        }
    }

    ConstIterator Begin() const {
        return ConstIterator(rep_);
    }

    ConstIterator End() const {
        return ConstIterator(rep_ + kArraySize);
    }

    static const size_t kArraySize = 64;
private:
    SimpleObject rep_[kArraySize];
};

TEST(Basic,Init) {
    TestArray l;

    auto it = l.Begin();
//    for (int i = 0; i < TestArray::kArraySize; i++) {
//        printf("%d\t",it->Value());
//        it++;
//    }
    auto end=l.End();
    printf("%d\t",end->Value());
//
//    ASSERT_EQ(it, l.End());

//    size_t cnt = 0;
//    for (it = l.Begin(); it != l.End(); it++) {
//        const SimpleObject &obj = *it;
//        printf("%d\t",obj.Value());
//        ASSERT_EQ(obj.Value(), cnt + 1);
//        cnt++;
//    }
//
//    /// May cause link error if we use ASSERT_EQ to compare the two values.
//    ASSERT_TRUE(cnt == TestArray::kArraySize);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}