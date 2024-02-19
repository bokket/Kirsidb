//
// Created by bokket on 2024/2/1.
//

#pragma once

#include <cstdint>

namespace bokket
{

//OffsetInfo
struct BlockHandle {
    int32_t size_=0;
    int32_t offset_=0;

    int32_t get_offset_info_size() {
        return sizeof(size_)+sizeof(offset_);
    }

    void clear() {
        size_=0;
        offset_=0;
    }
};

}
