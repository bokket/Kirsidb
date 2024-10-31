#pragma once

#include <type_traits>

//static_cast<Args && (*)() noexcept>(nullptr)()
#define URSPAWN_DECLVAL(...) static_cast<__VA_ARGS__(*)() noexcept>(nullptr)()

#define URSPAWN_IS_SAME(...) std::is_same<__VA_ARGS__>::value

namespace 
urspawn
{


template<class T>
struct remove_cvref
{
    using type=T;
};

template<class T>
struct remove_cvref<T const>
{
    using type=T;
};

template<class T>
struct remove_cvref<T&>
{
    using type=T;
};

template<class T>
struct remove_cvref<T const&>
{
    using type=T;
};

template <class T>
struct remove_cvref<T&&> 
{
  using type = T;
};

template <class T>
struct remove_cvref<T const&&> 
{
  using type = T;
};

template<class T>
using remove_cvref_t=typename remove_cvref<T>::type;


template<typename T,typename... Ts>
//折叠表达式
inline constexpr bool is_one_of_v=(URSPAWN_IS_SAME(T,Ts) || ...);


// template<bool B>
// struct _if
// {
//     template<typename ,typename T>
//     using apple=T;
// };

// template<>
// struct _if<true>
// {
//     template<typename T,typename >
//     using apple=T;
// };

// template<bool B,typename T,typename U>
// using conditional_t=typename _if<B>::template apple<T,U>;
template<bool B,typename T,typename U>
using conditional_t=std::conditional_t<B, T, U>;


template<template<typename ,typename...>class T>
struct meta_quote
{
    template<typename A,typename... B>
    using apply=T<A,B...>;

    template<typename...>
    struct bind_front;

    template<typename A,typename... B>
    struct bind_front<A,B...>
    {
        template<typename... C>
        using apply=T<A,B...,C...>;
    };
};

}