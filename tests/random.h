//
// Created by bokket on 24-3-25.
//

#pragma once
#include <random>

#include "../SkipList/comparator.h"

namespace bokket
{

std::mt19937 random_gen;

inline int RandomIn(int a, int b) {
    std::uniform_int_distribution<int> dis(a, b);
    return dis(random_gen);
}

inline std::string RandomString(int len) {
        std::string r;
        r.resize(len);
        for (int i = 0; i < len; i++)
            r[i] = RandomIn(' ', ' ' + 95);
        return r;
}

struct newByteCmp {
    const Comparator *cmp;

    newByteCmp()
            : cmp(newByteComparator()) {}

    bool operator()(std::string_view a, std::string_view b) const {
        return cmp->Compare(a, b) < 0;
    }
};

using kvMap=std::map<std::string,std::string,newByteCmp>;

}
