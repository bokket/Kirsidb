#pragma once

#include <concepts>
#include "concepts.h"
#include "blocking.h"
#include "tag_invoke.h"
#include "type_traits.h"
namespace 
urspawn::_just_done 
{

template<typename Receiver>
struct op
{
    struct type;
};

template<typename Receiver>
using operation=typename op<remove_cvref_t<Receiver>>::type;

template<typename Receiver>
struct op<Receiver>::type
{
    [[no_unique_address]] Receiver receiver_;

    void start() & noexcept
    {
        
    }
};

class sender
{

public:
    template<
        template<typename...>
        class Variant,
        template<typename...>
        class Tuple>

    using value_types=Variant<>;

    template<template<typename...>
        class Variant>
    using errors=Variant<>;

    static constexpr bool send_done=true;

    static constexpr auto blocking=blocking_kind::always_inline;

    template<typename This,typename Receiver>
        requires (same_as<remove_cvref_t<This>, sender> and
                 receiver<Receiver>)
        friend auot tag_invoke()
        
};


}