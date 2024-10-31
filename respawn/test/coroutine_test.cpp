#include <gtest/gtest.h>

#include <coroutine>
#include <print>
#include <type_traits>
#include <utility>


#include "../coroutine_concepts.h"

using namespace urspawn;


struct AwaiterWithMember
{
    void operator co_await() const
    {
        std::println("AwaiterWithMember void operator co_await() const");
    }
};

struct AwaiterWithFree{};

void operator co_await(AwaiterWithFree free) 
{
    std::println("void operator co_await(AwaiterWithFree free)");
}


struct Awaitable
{
    static bool await_ready() noexcept
    {
        return true;
    }

    static int await_resume() 
    {
        return 1;
    }
};

struct Awaitable_with_no_ready
{

    // static int await_resume() 
    // {
    //     return 1;
    // }
};

TEST(Awaitable,Awaitable_member_and_free)
{
    AwaiterWithMember member;
    get_awaiter(member);

    AwaiterWithFree free;
    get_awaiter(free);
}

TEST(AwaitableTypeTest,AwaiterType)
{
    using type=detail::await_result_impl<Awaitable>::type;
    std::println("{}",std::is_same_v<type, int>);

    // using no_ready_type=detail::await_result_impl_t<Awaitable_with_no_ready>;
    // std::println("{}",std::is_same<no_ready_type, detail::await_result_impl<Awaitable_with_no_ready>::type>::value);
    
    //false 
    // std::println("{}",std::is_same<no_ready_type,detail::await_result<Awaitable_with_no_ready>::type>::value);


    // std::println("{} {}",typeid(no_ready_type).name(),typeid(type).name());

    using awaiter_type=detail::awaiter_type<Awaitable>;

    using decl_awaiter_type=decltype(get_awaiter(std::declval<Awaitable>()));

    //std::is_same<awaiter_type, decl_awaiter_type>::value
    std::println("{} {}",typeid(decl_awaiter_type).name(),typeid(awaiter_type).name());

    std::println("{}",std::is_same<awaiter_type, decl_awaiter_type>::value);



    using await_result=detail::await_result_t<Awaitable>;
    using finally=detail::await_result_impl<detail::awaiter_type<Awaitable>>::type;
    std::println("{}",typeid(await_result).name());

    // std::println("{}",std::is_same_v<await_result, detail::await_result<Awaitable>>);


    // using result=detail::await_result_t<Awaitable_with_no_ready>;

    // std::println("{}",typeid(result).name());

    // std::println("{}",std::is_same_v<result, detail::await_result<Awaitable_with_no_ready>::type>);
}


struct ValidAwaitable
{
    struct Awaiter
    {
        bool await_ready() 
        {
            return true;
        }

        void await_suspend(std::coroutine_handle<>) {}

        void await_resume() {}
    };

    auto operator co_await() const noexcept
    {
        return Awaiter{};
    }
};


struct InvalidAwaitable {};


template<typename T>
void testAwaitable()
{
    if constexpr(detail::awaitable<T>) {
        std::println("T is awaitable.");
    } else {
        std::println("T is not awaitable.");
    }
}


TEST(AwaitableTest,Awaitable)
{
    testAwaitable<int>();
    testAwaitable<ValidAwaitable>();
    testAwaitable<InvalidAwaitable>();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}