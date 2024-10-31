#pragma once

#include <type_traits>
#include "blocking.h"


namespace urspawn
{

template<typename>
struct sender_traits;    

}


namespace urspawn::detail 
{

using urspawn::_block::_has_blocking;

template<typename Sender,bool = _has_blocking<Sender>::value>
struct _blocking:blocking_kind::constant<blocking_kind::maybe> {};


template<typename Sender>
struct _blocking<Sender,true>: blocking_kind::constant<Sender::blocking> {};


template<typename Sender,typename=void>
struct _has_is_always_scheduler_affine : std::false_type{};


template<typename Sender>
struct _has_is_always_scheduler_affine
    <Sender,
     std::void_t<decltype(Sender::is_always_scheduler_affine)>> : std::true_type{};

template<typename Sender,bool = _has_is_always_scheduler_affine<Sender>::value>
struct _is_always_scheduler_affine 
: std::bool_constant<blocking_kind::always_inline == _blocking<Sender>::value > 
{};

template<typename Sender>
struct _is_always_scheduler_affine<Sender,true>
: std::bool_constant<Sender::is_always_scheduler_affine>
{};

template<typename Sender>
struct _sender_traits
{
    template<template <typename...> class Variant,
             template <typename...> class Tuple>
    using value_types=typename Sender::template value_types<Variant,Tuple>;

    template<template<typename...> class Variant>
    using error_types=typename Sender::template error_types<Variant>;

    static constexpr bool sends_done=Sender::sends_done;

    static constexpr bool is_always_scheduler_affine = _is_always_scheduler_affine<Sender>::value;

    static constexpr blocking_kind blocking = _blocking<Sender>::value;        
};

template<typename Sender>
struct _bulk_sender_traits : _sender_traits<Sender>
{
    template<template<typename...> class Variant,
             template<typename...> class Tuple>
    using next_types=typename Sender::template next_types<Variant,Tuple>;
};

struct _no_sender_traits
{
    using _unspecialized = void;
};


template<typename Sender>
concept _has_sender_traits = !requires {
    typename sender_traits<Sender>::_unspecialized;
};

template<typename Sender>
constexpr auto _select_sender_traits() noexcept
{
    if constexpr(_has_bulk)
}



}