#pragma once

#include <utility>
#include <type_traits>

#include "./type_traits.h"

namespace uriex::tag_invoke
{

void tag_invoke();

struct fn
{
    template<typename CPO,typename... Args>
    constexpr auto operator()(CPO cpo,Args&&... args)const
        noexcept(noexcept(tag_invoke((CPO&&) cpo,(Args&&) args...)))
            ->decltype(tag_invoke((CPO&&) cpo,(Args&&) args...))
            {
                return tag_invoke((CPO&&) cpo,(Args&&) args...);
            };
};

// Ref: https://github.com/facebook/folly/blob/main/folly/functional/Invoke.h#L772
template<typename CPO,typename... Args>
using tag_invoke_result_t=
    decltype(tag_invoke(URIEX_DECLVAL(CPO&&),URIEX_DECLVAL(Args&&)...));

// template <typename Tag, typename... Args>
// using tag_invoke_result_t = decltype(tag_invoke(
//     static_cast<Tag && (*)() noexcept>(nullptr)(),
//     static_cast<Args && (*)() noexcept>(nullptr)()...));


using yes_type=char;
using no_type=char(&)[2];

template<typename CPO,typename... Args,typename =tag_invoke_result_t<CPO, Args...>>
char try_tag_invoke(int) noexcept(noexcept(tag_invoke(URIEX_DECLVAL(CPO&&),URIEX_DECLVAL(Args&&)...)));

template<typename CPO,typename... Args>
no_type try_tag_invoke(...) noexcept(false);





namespace cpo {
    inline constexpr fn tag_invoke{};
}

}


