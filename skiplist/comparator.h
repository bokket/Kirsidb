//
// Created by bokket on 2024/2/5.
//

#pragma once

#include <string>
#include <string_view>

#include "../db/export.h"

namespace bokket
{

class EXPORT Comparator
{
public:
    virtual ~Comparator();

    virtual int Compare(std::string_view a, std::string_view b) const=0;
};

}
