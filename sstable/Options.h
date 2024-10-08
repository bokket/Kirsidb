//
// Created by bokket on 24-4-10.
//

#pragma once

#include <cstddef>

namespace bokket {

struct Options
{
    int block_restart_interval;
    // 4M
    //std::size_t block_size=4*1024;
    std::size_t block_size = 12;
};

} // namespace bokket
