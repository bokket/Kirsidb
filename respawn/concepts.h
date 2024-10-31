#pragma once

#include <concepts>
namespace 
urspawn
{

template<typename A,typename B>
concept same_as= std::same_as<A, B> and std::same_as<B, A>;

}