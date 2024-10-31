#pragma once

#include <type_traits>
namespace 
urspawn
{

//基础模板：默认情况
template<typename T,typename=void>
inline constexpr bool is_stop_never_possible_v=false;

//特化模板：启用 SFINAE
//检查类型 T 是否具有 stop_possible() 方法，并且该方法返回 false。
//std::bool_constant<(T{}.stop_possible())>：这个表达式返回一个 std::true_type 或 std::false_type
//如果 T 有一个 stop_possible() 方法且它返回 false，那么 is_stop_never_possible_v<T> 为 true。否则是 false
//如果 T 没有定义这个函数，这里就会触发 SFINAE，模板匹配失败，退化到默认特化版本（即 is_stop_never_possible_v<T> 为 false）。
template<typename T>
inline constexpr bool is_stop_never_possible_v<
    T,
    std::enable_if_t<
        std::is_same_v<
            std::false_type,
            std::bool_constant<(T{}.stop_possible())>
        >
    >
> = true;


template<typename T>
using is_stop_never_possible=std::bool_constant<is_stop_never_possible_v<T>>;

}