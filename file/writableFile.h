//
// Created by bokket on 24-4-12.
//

#pragma once

#include "../db/status.h"

namespace bokket
{

class WritableFile
{
public:
    WritableFile()=default;
    virtual ~WritableFile() = default;

public:
    virtual Status append(std::string_view data) = 0;

    virtual Status sync() = 0;

    virtual Status close() = 0;

    virtual Status flush() = 0;
};

}
