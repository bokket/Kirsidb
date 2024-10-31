#pragma once


#include <type_traits>
#include <utility>

#include "urspawn_fwd.h"

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

namespace urspawn::detail 
{

template<typename Awaiter,typename =void>
struct await_result_impl{
};

template<typename Awaiter>
struct await_result_impl<Awaiter,std::void_t<decltype(std::declval<Awaiter&>().await_ready() ? 0:1  ),
                                             decltype(std::declval<Awaiter&>().await_resume())>>
{
    using type=decltype(std::declval<Awaiter&>().await_resume());
};

template<typename Awaiter>
using await_result_impl_t=typename await_result_impl<Awaiter>::type;


template<typename Awaitable>
using awaiter_type=decltype(get_awaiter(std::declval<Awaitable>()));


// template<typename Awaitable>
// using await_result_t=await_result_impl_t<awaiter_type<Awaitable>>;
// using await_result_t=typename await_result_impl<awaiter_type<Awaitable>>::type;

// warn don't use
// using await_result_t=await_result_impl_t<Awaitable>;

/// I don't know why.
/// T is awaitable.
/// T is awaitable.
/// T is awaitable. 

template<typename Awaitable,typename =void>
struct await_result
{
};

template<typename Awaitable>
// must await_result_impl type 
// std::void_t<await_result_impl_t<awaiter_type<Awaitable>>
struct await_result<Awaitable,std::void_t<typename await_result_impl<awaiter_type<Awaitable>>::type>>
{
    using type=std::conditional_t<
        std::is_same_v<await_result_impl<Awaitable>, await_result>,
        await_result_impl<awaiter_type<Awaitable>>,
        await_result>;
};

template<typename Awaitable>
using await_result_t=typename await_result<Awaitable>::type;





template<typename Awaitable>
concept awaitable_impl= requires(Awaitable a)
{
    typename await_result_t<Awaitable>;
};


template<typename Awaitable>
concept awaitable=awaitable_impl<Awaitable>;


}
