#pragma once

#include <type_traits>
#include "tag_invoke.h"
#include "type_traits.h"
#include "unstoppable_token.h"

//Kaori
namespace urspawn::_get_stop_token
{

struct fn
{
    template<typename T>
    requires(!tag_invocable<fn, const T&>)
    constexpr auto
    operator()(const T&) const noexcept
    ->unstoppable_token
    {
        return unstoppable_token{};
    }

    template<typename T>
    requires(tag_invocable<fn, const T&>)
    constexpr auto
    operator()(const T& object) const noexcept
    ->tag_invoke_result_t<fn, const T&>
    {
        static_assert(is_nothrow_tag_invocable_v<fn, const T&>,
        "_get_stop_token() cpo must be declared noexcept" );

        return tag_invoke(fn{},object);
    }
};

inline const struct fn get_stop_token{};
}


namespace urspawn
{

using _get_stop_token::get_stop_token;


template<typename T>
using get_stop_token_result_t=std::invoke_result_t<tag_t<get_stop_token>,T>;


template<typename Receiver>
using stop_token_type_t=remove_cvref_t<get_stop_token_result_t<Receiver>>;


}