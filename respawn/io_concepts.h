#pragma once

#include "tag_invoke.h"



namespace urspawn::io_cpo 
{


struct async_read_some_cpo
{
    template<typename ForwardReader,typename Buffer>
    auto operator()(ForwardReader& socket,Buffer&& buffer) const
        noexcept(is_nothrow_tag_invocable_v<async_read_some_cpo, ForwardReader&,Buffer>)
    -> _tag_invoke::tag_invoke_result_t<async_read_some_cpo, ForwardReader&,Buffer>
    {
        return urspawn::_tag_invoke::tag_invoke(*this,socket,(Buffer&&)buffer);
    }

    template<typename ForwardReader>
    auto operator()(ForwardReader& socket) const
        noexcept(is_nothrow_tag_invocable_v<async_read_some_cpo, ForwardReader&>)
    ->_tag_invoke::tag_invoke_result_t<async_read_some_cpo, ForwardReader&>
    {
        return urspawn::_tag_invoke::tag_invoke(*this,socket);
    }

};

struct async_write_some_cpo
{
    template<typename ForwardWriter,typename Buffer>
    auto operator()(ForwardWriter& socket,Buffer&& buffer) const
        noexcept(is_nothrow_tag_invocable_v<async_write_some_cpo, ForwardWriter&,Buffer>)
    -> _tag_invoke::tag_invoke_result_t<async_write_some_cpo, ForwardWriter&,Buffer>
    {
        return urspawn::_tag_invoke::tag_invoke(*this,socket,(Buffer&&)buffer);
    }

    template<typename ForwardWriter>
    auto operator()(ForwardWriter& socket) const
        noexcept(is_nothrow_tag_invocable_v<async_write_some_cpo, ForwardWriter&>)
    ->_tag_invoke::tag_invoke_result_t<async_write_some_cpo, ForwardWriter&>
    {
        return urspawn::_tag_invoke::tag_invoke(*this,socket);
    }

};


struct async_read_at_cpo
{
    template<typename RandomReader,typename Buffer>
    auto  operator()(
        RandomReader& file,
        typename RandomReader::offset_t offset,
        Buffer&& buffer) const
    noexcept(is_nothrow_tag_invocable_v<async_read_at_cpo,RandomReader&,typename RandomReader::offset_t,Buffer>)
    ->tag_invoke_result_t<async_read_at_cpo,RandomReader&,typename RandomReader::offset_t,Buffer>
    {
        return tag_invoke(*this,file,offset,(Buffer&&)buffer);
    }

    template<typename RandomeReader>
    auto operator()(RandomeReader& file) const
    noexcept(is_nothrow_tag_invocable_v<async_read_at_cpo, RandomeReader&>) 
    ->tag_invoke_result_t<async_read_at_cpo, RandomeReader&>
    {
        return tag_invoke(*this,file);
    }

};

struct async_write_at_cpo
{
    template<typename RandomWriter,typename Buffer>
    auto  operator()(
        RandomWriter& file,
        typename RandomWriter::offset_t offset,
        Buffer&& buffer) const
    noexcept(is_nothrow_tag_invocable_v<async_write_at_cpo,RandomWriter&,typename RandomWriter::offset_t,Buffer>)
    ->tag_invoke_result_t<async_write_at_cpo,RandomWriter&,typename RandomWriter::offset_t,Buffer>
    {
        return tag_invoke(*this,file,offset,(Buffer&&)buffer);
    }

    template<typename RandomWriter>
    auto operator()(RandomWriter& file) const
    noexcept(is_nothrow_tag_invocable_v<async_write_at_cpo, RandomWriter&>) 
    ->tag_invoke_result_t<async_write_at_cpo, RandomWriter&>
    {
        return tag_invoke(*this,file);
    }

};


inline const async_read_some_cpo 
async_read_some{};


inline const async_write_some_cpo
async_write_some{};

inline const async_read_at_cpo
async_read_at{};

inline const async_write_at_cpo
async_write_at{};

}

namespace urspawn
{

using io_cpo::async_read_some;
using io_cpo::async_write_some;
using io_cpo::async_read_at;
using io_cpo::async_write_at;


}

