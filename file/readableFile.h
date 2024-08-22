//
// Created by bokket on 24-4-12.
//

#pragma once

#include "../db/status.h"

namespace bokket
{

class ReadableFile
{
public:
    ReadableFile() = default;
    virtual ~ReadableFile() = default;

    virtual Status read(uint64_t offset, size_t n, std::string *result) = 0;
};


}
