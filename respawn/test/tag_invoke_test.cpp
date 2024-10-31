#include <gtest/gtest.h>

#include <print>
#include <type_traits>


#include "../tag_invoke.h"


using namespace urspawn;

inline constexpr struct test_cpo
{

}test;

struct test_cpo1
{

};


struct X
{
    friend void tag_invoke(test_cpo,X){
        std::println("{}","X::tag_invoke(test_cpo,X)");
    }
    friend constexpr bool tag_invoke(test_cpo,X,int a) noexcept {
        std::println("{} {}","X::bool tag_invoke(test_cpo,X,int a)",a);
        return a>0;
    }
};

struct Y{};


struct W
{
    void operator()(test_cpo,W)
    {
         std::println("{}","W::operator()(test_cpo,W)");
    }

    bool operator()(test_cpo,W,double b) noexcept 
    {
        std::println("{} {}","W::bool operator()(test_cpo,W,int a)",b);
        return b>0;
    }
};


struct test_cpo2
{
    void operator()(int) {}

    void operator()(test_cpo2,int) {}
};

// tag_invoke 实现
inline void tag_invoke(test_cpo2, int) {
}

template<typename CPO>
void test_tag_invocable() {
    if constexpr(tag_invocable<CPO, int>) {
        std::println("{}","test_tag_invocable tag_invocable<CPO, int>");
    } 
    else 
    {
        std::println("{}","test_tag_invocable not tag_invocable");
    }
}

TEST(tag_invocable,test_tag_invocable)
{
    test_tag_invocable<test_cpo2>();


    test_tag_invocable<W>();

    test_tag_invocable<X>();
}



TEST(tag_invoke,tag_invoke_usage)
{
    _tag_invoke::cpo::tag_invoke(test,X{});

    EXPECT_TRUE(_tag_invoke::cpo::tag_invoke(test_cpo{},X{},42));

    EXPECT_FALSE(_tag_invoke::cpo::tag_invoke(test,X{},-1));

    std::println("{}",is_tag_invocable_v<test_cpo, X>);

    //static_assert(std::is_same_v<int,tag_invoke_result_t<test_cpo2,test_cpo2,test_cpo2, int>>);

    // static_assert(std::is_same_v<void, tag_invoke_result_t<W, test_cpo,W>>);

    // static_assert(std::is_same_v<bool, tag_invoke_result_t<W, test_cpo,W,double>>);

    static_assert(std::is_same_v<void,_tag_invoke::tag_invoke_result_t<test_cpo,X>> );

    static_assert(std::is_same_v<bool,_tag_invoke::tag_invoke_result_t<test_cpo,X,int>> );

    //   在类型转换之后试图调用该函数，通常不会在运行时执行，只用于类型推导或静态分析。
    //?  static_cast<__VA_ARGS__(*)() noexcept>(nullptr)()
    //   表示一个空的函数指针，不会实际调用函数，仅进行类型转换。
    //   static_cast<__VA_ARGS__(*)() noexcept>(nullptr) 区别

    static_assert(!is_tag_invocable_v<test_cpo, Y>, "Test failed");

 
    static_assert(is_tag_invocable_v<test_cpo, X>, "Test failed");

    static_assert(is_tag_invocable_v<test_cpo, X,int>, "Test failed");

    static_assert(is_nothrow_tag_invocable_v<test_cpo, X,int>);

    static_assert(!is_nothrow_tag_invocable_v<test_cpo, X>);

    static_assert(!is_nothrow_tag_invocable_v<test_cpo, Y>);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


