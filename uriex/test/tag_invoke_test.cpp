#include <gtest/gtest.h>


#include "../tag_invoke.h"

inline constexpr struct test_cpo
{

}test;



struct X
{
    friend void tag_invoke(test_cpo,X){}
    friend constexpr bool tag_invoke(test_cpo,X,int a) noexcept {
        return a>0;
    }
};

struct Y{};


TEST(tag_invoke,tag_invoke_usage)
{
    uriex::tag_invoke::cpo::tag_invoke(test,X{});
    EXPECT_TRUE(uriex::tag_invoke::cpo::tag_invoke(test_cpo{},X{},42));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


