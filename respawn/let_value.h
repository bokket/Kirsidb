#pragma once

#include <tuple>
#include <type_traits>
namespace urspawn::_let_value
{


//将类型 Values... 转换为 "衰减" 类型
template<typename... Values>
using decayed_tuple=std::tuple<std::decay_t<Values>...>;


template<typename Operation,typename... Values>
struct _successor_receiver
{
    struct type;
};

template<typename Operation,typename... Values>
using successor_receiver=
    typename _successor_receiver<Operation, Values...>::type;

template<typename Operation,typename... Values>
struct _successor_receiver<Operation, Values...>::type
{
    using successor_receiver=type;

    Operation& op_;

    typename Operation::receiver_type& get_receiver() const noexcept
    {
        return op_.receiver_;
    }

    template<typename... SuccessorValues>
    void set_value(SuccessorValues&&... values) && noexcept
    {
        set_value(SuccessorValues &&values...)
    }

};



}