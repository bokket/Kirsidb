//
// Created by bokket on 24-3-25.
//

#include "comparator.h"

#include <mutex>


namespace bokket
{

class byteComparator:public Comparator
{
public:
    byteComparator()=default;

    int Compare(std::string_view a, std::string_view b) const override {
        return a.compare(b);
    }
};

static const Comparator* byteCmp= nullptr;

static void InitModule() {
    byteCmp=new byteComparator();
}

const Comparator* newByteComparator() {
    static std::once_flag flag;
    std::call_once(flag, InitModule);
    return byteCmp;
}


}



