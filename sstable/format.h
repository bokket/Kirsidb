//
// Created by bokket on 2024/2/1.
//

#pragma once

#include <cstdint>


#include "../Log/Log.h"
#include "../db/status.h"

namespace bokket
{

//OffsetInfo
struct BlockHandle {

    BlockHandle()
               :offset_{0}
               ,size_{0}
    {}

    BlockHandle(uint64_t size, uint64_t offset)
               :size_{size}
               ,offset_{offset}
    {}

    void Encode(std::string& output, const BlockHandle& handle);

    std::string EncodeToString() const;

    static Status DecodeFrom(const char* input, BlockHandle& handle);

    std::string DebugString(const BlockHandle& handle);

    void DebugString();

    uint64_t get_offset_info_size() {
        return sizeof(size_)+sizeof(offset_);
    }

    void clear() {
        size_=0;
        offset_=0;
    }

    uint64_t size_;
    uint64_t offset_;
};

}
