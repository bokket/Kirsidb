#pragma once

#include <type_traits>
#include <utility>
#include <coroutine>

namespace urspawn::detail
{

template<typename Awaitable,typename =void>
constexpr bool has_member_operator_co_await_v=false;

template<typename Awaitable>
constexpr bool has_member_operator_co_await_v<Awaitable,
                                             std::void_t<decltype(std::declval<Awaitable>().operator co_await())>> = true ;

template<typename Awaitable,typename =void>
constexpr bool has_free_operator_co_await_v=false;

template<typename Awaitable>
constexpr bool has_free_operator_co_await_v<Awaitable,
                                            std::void_t<decltype(operator co_await(std::declval<Awaitable>()))>> =true;

}

namespace urspawn::_get_awaiter
{

struct fn
{
    template<typename Awaitable>
    constexpr decltype(auto) operator()(Awaitable&& awaitable) const noexcept
    {
        if constexpr(detail::has_member_operator_co_await_v<Awaitable>)
        {
            return static_cast<Awaitable&&>(awaitable).operator co_await();
        }
        else if constexpr(detail::has_free_operator_co_await_v<Awaitable>)
        {
            return operator co_await(static_cast<Awaitable&&>(awaitable));
        } else {
            return static_cast<Awaitable&&>(awaitable);
        }
    }

};


}


namespace urspawn 
{

inline constexpr _get_awaiter::fn get_awaiter{};

}

namespace  urspawn::detail
{

template<typename T>
concept convertible_to_boolean_impl=std::convertible_to<T, bool>;

// need experimental verification
template<typename T>
concept boolean_testable = convertible_to_boolean_impl<T> and requires (T&& t) {
    { not static_cast<T&&>(t) } -> convertible_to_boolean_impl;
};

template<typename Awaiter>
concept is_awaiter=requires(Awaiter a) 
{
    {a.await_ready()} -> convertible_to_boolean_impl;
    a.await_resume();
};


template<typename Awaiter>
concept awaiter = is_awaiter<Awaiter>;

template<typename Awaitable>
using awaiter_type=decltype(get_awaiter(std::declval<Awaitable>()));

template<typename Awaitable>
concept awaitable_impl = is_awaiter<awaiter_type<Awaitable>>;

template<typename Awaitable>
concept awaitable = awaitable_impl<Awaitable>;

}

namespace urspawn::detail 
{

template<typename Awaiter>
struct await_result_impl;

template<typename Awaiter> 
requires (awaiter<Awaiter>)
struct await_result_impl<Awaiter> {
    using type = decltype(std::declval<Awaiter&>().await_resume());
};


template<typename Awaiter> 
requires (!awaiter<Awaiter>)
struct await_result_impl<Awaiter> {
    using type = void;
};

template<typename Awaiter>
using await_result_impl_t=typename await_result_impl<Awaiter>::type;

template<typename Awaiter>
using await_result_t=typename await_result_impl<Awaiter>::type;

}
