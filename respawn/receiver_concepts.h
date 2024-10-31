#pragma once

#include <type_traits>
#include "tag_invoke.h"
#include "type_traits.h"


namespace urspawn::_receiver_cpo
{

struct set_value_fn
{
    private:
        template<typename Receiver,typename... Values>
        using set_value_member_result_t =
            decltype(URSPAWN_DECLVAL(Receiver).set_value(URSPAWN_DECLVAL(Values)...));
        
        template<typename Receiver,typename... Values>
        using result_t=typename conditional_t
        <tag_invocable<set_value_fn, Receiver,Values...>,
         meta_tag_invoke_result<set_value_fn>,
         meta_quote<set_value_member_result_t>>::template apply<Receiver,Values...>;

    public:
        template<typename Receiver,typename... Values>
            requires tag_invocable<set_value_fn, Receiver,Values...>
            auto operator()(Receiver&& r,Values&&... values) const
            noexcept(is_nothrow_tag_invocable_v<set_value_fn, Receiver,Values...>)
            ->result_t<Receiver, Values...>
            {
                return tag_invoke(set_value_fn{},(Receiver&&)r,(Values&&)values...);
            }

        template<typename Receiver,typename... Values>
            requires (!tag_invocable<set_value_fn, Receiver,Values...>)
            auto operator()(Receiver&&r,Values&&... values) const
            noexcept(noexcept(static_cast<Receiver&&>(r).set_value((Values&&)values...)))
                ->result_t<Receiver, Values...>
            {
                return static_cast<Receiver&&>(r).set_value((Values&&)values...);
            }
};


struct set_next_fn 
{
    private:
        template<typename Receiver,typename... Values>
        using set_next_member_result_t=
            decltype(URSPAWN_DECLVAL(Receiver&).set_next(URSPAWN_DECLVAL(Values)...));

    template<typename Receiver,typename... Values>
    using result_t=typename conditional_t
    <tag_invocable<set_next_fn, Receiver&,Values...>,
     meta_tag_invoke_result<set_next_fn>,
     meta_quote<set_next_member_result_t>>::template apply<Receiver,Values...>;

    public:
        template<typename Receiver,typename... Values>
            requires(tag_invocable<set_next_fn, Receiver,Values...>)
            auto operator()(Receiver&r,Values&&... values) const
            noexcept(is_nothrow_tag_invocable_v<set_next_fn, Receiver,Values...>)
            ->result_t<set_next_fn, Receiver,Values...>
        {
            return tag_invoke(set_next_fn{},r,(Values&&)values...);
        }

        template<typename Receiver,typename... Values>
            requires(!tag_invocable<set_next_fn, Receiver&,Values...>)
            auto operator()(Receiver&r,Values&&... values) const
            noexcept(noexcept(r.set_next((Values&&)values...)))
            ->result_t<Receiver, Values...>
        {
            return r.set_next((Values&&)values...);
        }

};


struct set_error_fn
{
    private:
        template<typename Receiver,typename Error>
        using set_error_member_result_t=
            decltype(URSPAWN_DECLVAL(Receiver).set_error(URSPAWN_DECLVAL(Error)));

        template<typename Receiver,typename Error>
        using result_t=typename conditional_t
        <tag_invocable<set_error_fn, Receiver,Error>, 
         meta_tag_invoke_result<set_error_fn>,
         meta_quote<set_error_member_result_t>>::template apply<Receiver,Error>;

    public:
        template<typename Receiver,typename Error>
            requires(tag_invocable<set_error_fn, Receiver,Error>)
            auto operator()(Receiver&& r,Error&& error) const
            ->result_t<Receiver,Error>
        {
            static_assert(is_nothrow_tag_invocable_v<set_error_fn, Receiver,Error>,
            "Receiver set_error() invocation is required to be noexcept." );

            static_assert(std::is_void_v<tag_invoke_result_t<set_error_fn, Receiver,Error>>);
        
            return tagag_invoke(set_error_fn{},(Receiver&&)r,(Error&&)error);
        }

        template<typename Receiver,typename Error>
            requires(!tag_invocable<set_error_fn, Receiver,Error>)
            auto operator()(Receiver&&r,Error&& error) const noexcept
            ->result_t<Receiver, Error>
        {
            static_assert(noexcept(static_cast<Receiver&&>(r).set_error((Error&&)error)),
            "Receiver .set_error() method must be nothrow invocable" );

            return static_cast<Receiver&&>(r).set_error((Error&&)error);
        }
};


struct set_done_fn
{
    private:
        template<typename Receiver>
        using set_done_member_result_t=
            decltype(URSPAWN_DECLVAL(Receiver).set_done());

        template<typename Receiver>
        using result_t=typename conditional_t
        <tag_invocable<set_done_fn, Receiver>, 
        meta_tag_invoke_result<set_done_fn>, 
        meta_quote<set_done_member_result_t>>::template apply<Receiver>;

    public:
        template<typename Receiver>
            requires(tag_invocable<set_done_fn, Receiver>)
            auto operator()(Receiver&& r) const 
            noexcept 
            ->result_t<Receiver>
        {
            static_assert(is_nothrow_tag_invocable_v<set_done_fn, Receiver>,
            "Receiver set_done() invocable is required to be noexcept." );

            static_assert(std::is_void_v<tag_invoke_result_t<set_done_fn, Receiver>>);

            return tag_invoke(set_done_fn{},(Receiver&&)r);
        }

        template<typename Receiver>
            requires(!tag_invocable<set_done_fn, Receiver>)
            auto operator()(Receiver&& r) const
            noexcept
            ->result_t<Receiver>
        {
            static_assert(
            noexcept(static_cast<Receiver&&>(r).set_done()),
            "Receiver.set_done() method must be nothrow invocable");
        
            return static_cast<Receiver&&>(r).set_done();
        }
};



inline const set_value_fn set_value{};
inline const set_next_fn set_next{};
inline const set_error_fn set_error{};
inline const set_done_fn set_done{};

}

namespace urspawn
{

using _receiver_cpo::set_value;
using _receiver_cpo::set_next;    
using _receiver_cpo::set_error;
using _receiver_cpo::set_done;



template<typename T>
inline constexpr bool is_one_of_receiver_cpo_v=is_one_of_v
<remove_cvref_t<T>, 
_receiver_cpo::set_value_fn,
_receiver_cpo::set_next_fn,
_receiver_cpo::set_error_fn,
_receiver_cpo::set_done_fn>;

template<typename T>
using is_receiver_cpo=std::bool_constant<is_one_of_receiver_cpo_v<T>>;



}