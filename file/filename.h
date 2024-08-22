#pragma once

#include <cstdint>
#include <string_view>

#include <format>

namespace bokket 
{

std::string SSTableFileName(std::string_view dbname,uint64_t number);    

}