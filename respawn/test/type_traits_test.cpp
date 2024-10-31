#include <fmt/core.h>
#include <gtest/gtest.h>
#include <type_traits>

#include "../type_traits.h"

using namespace urspawn;


template<typename A,typename B>
struct test_class
{
    using type1=A;
    using type2=B;
};

TEST(Meta_quote_test,Apply)
{
    using apply_type=meta_quote<test_class>::apply<int, float>;

    static_assert(std::is_same<apply_type::type1, int>::value,"type1 should be int" );

    static_assert(std::is_same<apply_type::type2, float>::value,"type2 should be float" );
}

TEST(Meta_quote_test,BindFront)
{
    using bind_type=meta_quote<test_class>::bind_front<int>::apply<float>;

    using front_type=meta_quote<test_class>::bind_front<int>::template apply<float>;

    static_assert(std::is_same<bind_type::type1, int>::value,"type1 should be int" );

    static_assert(std::is_same<bind_type::type2, float>::value,"type2 should be float" );
}

TEST(Conditional_test,True)
{
    EXPECT_TRUE((std::is_same<conditional_t<true, int, double>,int>::value));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
