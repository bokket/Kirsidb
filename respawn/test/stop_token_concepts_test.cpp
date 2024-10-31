#include <gtest/gtest.h>

#include <format>
#include <print>

#include "../get_stop_token.h"
#include "../stop_token_concepts.h"

using namespace urspawn;


struct Stoppable
{
    [[nodiscard]] static constexpr bool stop_possible()  
    {
        return true;
    }
};

struct NonStoppable
{
    [[nodiscard]] static constexpr bool stop_possible()  
    {
        return false;
    }
};


struct StoppableWithout
{
};



struct MyType
{

    std::string tag_invoke(_get_stop_token::fn,const MyType&) noexcept
    {
        return "MyType token";
    }

    friend int tag_invoke(_get_stop_token::fn,const MyType&) noexcept {
        std::println("MyType int tag_invoke");
        return 1;
    }
    
};

struct MyTest
{

    friend class MyType;

    int tag_invoke(_get_stop_token::fn,const MyTest&) noexcept
    {
        return 1;
    }

    friend std::string tag_invoke(_get_stop_token::fn, const MyTest&) noexcept {
        return "MyTest token: " ;
    }
};


// int tag_invoke(_get_stop_token::fn,const MyType&) noexcept
// {
//     return 1;
// }


// std::string tag_invoke(_get_stop_token::fn, const MyTest&) noexcept {
//     return "MyTest token";
// }



TEST(StopNeverPossibleTest, TypeWithStopPossible)
{
    EXPECT_FALSE(is_stop_never_possible_v<Stoppable>);

    EXPECT_TRUE(is_stop_never_possible_v<NonStoppable>);

    EXPECT_FALSE(is_stop_never_possible_v<StoppableWithout>);
}

TEST(StopNeverPossibleTest, StopPossible)
{
    EXPECT_FALSE(is_stop_never_possible<Stoppable>::value);

    EXPECT_TRUE(is_stop_never_possible<NonStoppable>::value);

    EXPECT_FALSE(is_stop_never_possible<StoppableWithout>::value);

    std::println("{}",is_stop_never_possible<NonStoppable>()());
}

TEST(Get_stop_token_test,TagInvocableTest)
{
    MyTest obj;
    auto token=get_stop_token(obj);

    std::println("{}",token);

    // auto t="MyType token";
    // auto s=get_stop_token(t);

    // EXPECT_EQ(s, "MyType token");

    //MyTest t;
    auto stop=get_stop_token(obj); // no
    std::println("{}",stop);
    // EXPECT_EQ(stop, 1);


    MyType t;
    auto type=get_stop_token(t);
    std::println("{}",type);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}