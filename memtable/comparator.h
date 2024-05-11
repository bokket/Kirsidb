//
// Created by bokket on 2024/2/5.
//

#pragma once

#include <mutex>
#include <string_view>

#include "../db/export.h"

namespace bokket
{

class EXPORT Comparator
{
public:
    Comparator()=default;

    virtual ~Comparator()=default;

    [[nodiscard]] virtual int Compare(std::string_view a, std::string_view b) const=0;
};

extern const Comparator *newByteComparator();

}
