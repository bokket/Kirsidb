//
// Created by bokket on 24-3-25.
//

#pragma once

#include <cstring>
#include <cassert>
#include <string>
#include <string_view>
#include <cstddef>

namespace bokket
{

class Buf {
public:
    Buf(const std::string& s)
       :Buf(s.data(),s.size())
    {}

    Buf(const char* s)
      :Buf(s,strlen(s))
    {}

    Buf(std::string_view s)
       : Buf(s.data(),s.size())
    {}

    constexpr Buf(const char* s,size_t n)
                :data_(s)
                ,size_(n)
    {}

    constexpr Buf(std::nullptr_t p= nullptr)
                 :data_(nullptr)
                 ,size_(0)
    {}

    size_t size() const {
        return size_;
    }

    const char *data() const {
        return data_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void skip(size_t n) {
        assert(n<=size_);
        data_+=n;
        size_-=n;
    }
private:
    const char* data_;
    std::size_t size_;
};

inline std::ostream &operator<<(std::ostream &stream, const Buf &buf) {
    return stream << buf.data();
}


}
