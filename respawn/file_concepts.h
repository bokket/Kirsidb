#pragma once

#include <filesystem>
#include <type_traits>
#include "tag_invoke.h"


namespace urspawn::file_cpo
{


using namespace std::filesystem;

struct open_file_read_only_cpo
{
    template<typename Executor>
    auto operator()(Executor&& executor,const std::filesystem::path& path) const
        noexcept(is_nothrow_tag_invocable_v<open_file_read_only_cpo, Executor,const std::filesystem::path&>)
    ->tag_invoke_result_t<open_file_read_only_cpo,Executor,const std::filesystem::path& >
    {
        return tag_invoke(*this,(Executor&&)executor,path);
    }
};


struct open_file_write_only_cpo
{
    template<typename Executor>
    auto operator()(Executor&& executor,const std::filesystem::path& path) const
        noexcept(is_nothrow_tag_invocable_v<open_file_write_only_cpo, Executor,const std::filesystem::path&>)
    ->tag_invoke_result_t<open_file_write_only_cpo,Executor,const std::filesystem::path& >
    {
        return tag_invoke(*this,(Executor&&)executor,path);

    }
};


struct open_file_read_and_write_cpo
{
    template<typename Executor>
    auto operator()(Executor&& executor,const std::filesystem::path& path) const
        noexcept(is_nothrow_tag_invocable_v<open_file_read_and_write_cpo, Executor,const std::filesystem::path&>)
    ->tag_invoke_result_t<open_file_read_and_write_cpo,Executor,const std::filesystem::path& >
    {
        return tag_invoke(*this,(Executor&&)executor,path);
    }
};


inline const open_file_read_only_cpo 
open_file_read_only{};


inline const open_file_write_only_cpo 
open_file_write_only{};


inline const open_file_read_and_write_cpo 
open_file_read_and_write{};


}


namespace  urspawn
{

using file_cpo::open_file_read_only;
using file_cpo::open_file_write_only;
using file_cpo::open_file_read_and_write;

}