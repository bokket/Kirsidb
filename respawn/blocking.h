#pragma once

#include <type_traits>
#include "tag_invoke.h"
namespace urspawn::_block
{

enum class block:unsigned char
{
    always_inline=0,

    always,

    maybe,

    never,
};

constexpr bool operator<(block lhs,block rhs) noexcept
{
    return static_cast<unsigned char>(lhs) < static_cast<unsigned char>(rhs);
}

struct blocking_kind
{
    block value{block::maybe};

    template<block Kind>
    //block 为enum class类型 
    //Kind  为具体值
    //通过 std::integral_constant，可以在编译时做出不同的选择。结合 std::conditional 可以实现编译时的类型分支
    using constant=std::integral_constant<block, Kind>;

    blocking_kind()=default;

    constexpr explicit blocking_kind(block Kind) noexcept:value(Kind){}
    
    template<block Kind>
    constexpr explicit blocking_kind(constant<Kind>) noexcept:value(Kind){}

    //显示转换 static_cast<block>()
    constexpr explicit operator block() const noexcept
    {
        return value;
    }

    //函数调用运算符  ()
    constexpr block operator ()() const noexcept
    {
        return value;
    }

    static constexpr constant<block::maybe> maybe{};

    static constexpr constant<block::never> never{};

    static constexpr constant<block::always> always{};
    
    static constexpr constant<block::always_inline> always_inline{};
};


template<typename Sender,typename=void>
struct _has_blocking : std::false_type{};


//std::void_t 用于检测类型 T Sender 是否有一个名为 type blocking的类型成员。
//如果有，则通过 SFINAE 机制选择特化版本；否则，使用默认版本。
template<typename Sender>
struct _has_blocking<Sender,std::void_t<decltype(Sender::blocking)>>
    :std::true_type{};

struct fn
{
    template<typename Sender>
    requires tag_invocable<fn, const Sender&>
        constexpr auto 
        operator()(const Sender& sender) const
        noexcept(is_nothrow_tag_invocable_v<fn, const Sender&>)
        ->tag_invoke_result_t<fn, const Sender&>
    {
        return tag_invoke(fn{},sender);
    }

    template<typename Sender>
    requires (!tag_invocable<fn, const Sender&>)
        constexpr auto
        operator()(const Sender& sender) const noexcept
    {
        if constexpr(_has_blocking<Sender>::value)
        {
            return blocking_kind::constant<Sender::blocking>{};
        } else {
            return blocking_kind::maybe;
        }
    }

};

}

namespace urspawn
{

inline constexpr _block::fn blocking{};
using _block::blocking_kind;

}