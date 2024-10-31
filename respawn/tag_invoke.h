#pragma once

#include <concepts>
#include <utility>
#include <type_traits>
#include <print>

#include "urspawn_fwd.h"
#include "type_traits.h"

namespace urspawn::_tag_invoke
{

void tag_invoke();

struct fn
{
    template<typename CPO,typename... Args>
    constexpr auto operator()(CPO cpo,Args&&... args)const
        noexcept(noexcept(tag_invoke((CPO&&) cpo,(Args&&) args...)))
            ->decltype(tag_invoke((CPO&&) cpo,(Args&&) args...))
            {
                std::println("namespace urspawn::_tag_invoke struct fn");
                return tag_invoke((CPO&&) cpo,(Args&&) args...);
            };
};

// Ref: https://github.com/facebook/folly/blob/main/folly/functional/Invoke.h#L772
template<typename CPO,typename... Args>
using tag_invoke_result_t=
    decltype(tag_invoke(URSPAWN_DECLVAL(CPO&&),URSPAWN_DECLVAL(Args&&)...));
// using tag_invoke_result_t=
//     std::invoke_result_t<CPO, Args...>;

// template <typename Tag, typename... Args>
// using tag_invoke_result_t = decltype(tag_invoke(
//     static_cast<Tag && (*)() noexcept>(nullptr)(),
//     static_cast<Args && (*)() noexcept>(nullptr)()...));


using yes_type=char;
using no_type=char(&)[2];

template<
    typename CPO,
    typename... Args,
    typename =tag_invoke_result_t<CPO, Args...>>
char try_tag_invoke(int) noexcept(noexcept(tag_invoke(URSPAWN_DECLVAL(CPO&&),URSPAWN_DECLVAL(Args&&)...)));
// char try_tag_invoke(int) noexcept(noexcept(tag_invoke(URSPAWN_DECLVAL(CPO&&),URSPAWN_DECLVAL(Args&&)...)));

template<typename CPO,typename... Args>
no_type try_tag_invoke(...) noexcept(false);





namespace cpo {
    inline constexpr fn tag_invoke{};
}

// 延迟模板实例化
template<template<typename ...>class T,typename... Args>
struct defer
{
    using type=T<Args...>;
};

}



namespace urspawn
{

using namespace _tag_invoke;



template<auto& CPO>
using tag_t=remove_cvref_t<decltype(CPO)>;


// template<typename CPO,typename... Args>
// inline constexpr bool is_tag_invocable_v =
//     (
//         sizeof(tag_invoke::try_tag_invoke<CPO,Args...>(0))==
//         sizeof(tag_invoke::yes_type)
//     );

// template <typename CPO, typename... Args>
// inline constexpr bool is_tag_invocable_v =
//     (sizeof(tag_invoke::try_tag_invoke<CPO, Args...>(0)) ==
//      sizeof(tag_invoke::yes_type));


template<typename CPO, typename... Args>
inline constexpr bool is_tag_invocable_v =
    (sizeof(_tag_invoke::try_tag_invoke<CPO, Args...>(0)) == sizeof(_tag_invoke::yes_type));
    

template<typename CPO,typename... Args>
using is_tag_invocable=std::bool_constant<is_tag_invocable_v<CPO, Args...>>;


template<typename CPO,typename... Args>
inline constexpr bool is_nothrow_tag_invocable_v =
    noexcept(_tag_invoke::try_tag_invoke<CPO,Args...>(0));

template<typename CPO,typename... Args>
using is_nothrow_tag_invocable=std::bool_constant<is_nothrow_tag_invocable_v<CPO, Args...>>;



// template<>
// detail::empty
// 配合其他模板工具 std::conditional来实现编译时的类型推导或分支选择。
template<typename CPO, typename... Args>
struct tag_invoke_result:
conditional_t<is_tag_invocable_v<CPO, Args...>, 
_tag_invoke::defer<_tag_invoke::tag_invoke_result_t, CPO,Args ...>,
detail::empty<>>
{};

// template <typename CPO, typename... Args>
// concept tag_invocable =
//     (sizeof(try_tag_invoke<CPO,Args...>(0))==
//      sizeof(yes_type));

template<typename CPO,typename... Args>
concept tag_invocable=
    requires{
        { try_tag_invoke<CPO,Args...>(0) } -> std::same_as<yes_type>;
    };


template<typename Fn>
using meta_tag_invoke_result=
    meta_quote<tag_invoke_result_t>::bind_front<Fn>;
    

}



