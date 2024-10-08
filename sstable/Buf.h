//
// Created by bokket on 24-3-25.
//

#pragma once

#include <cstring>
#include <cassert>
#include <string>
#include <string_view>
#include <cstddef>
#include <iostream>

namespace bokket
{

class Buf {
public:
    explicit Buf(const std::string& s)
       :Buf(s.data(),s.size())
    {}

    explicit Buf(const char* s)
      :Buf(s,strlen(s))
    {}

    explicit Buf(std::string_view s)
       : Buf(s.data(),s.size())
    {}

    constexpr Buf(const char* s,size_t n)
                :data_(s)
                ,size_(n)
    {}

    constexpr explicit Buf(std::nullptr_t p= nullptr)
                 :data_(nullptr)
                 ,size_(0)
    {}

    [[nodiscard]] size_t size() const {
        return size_;
    }

    [[nodiscard]] const char *data() const {
        return data_;
    }

    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    const char &operator[](size_t n) const {
        return data_[n];
    }

    void skip(size_t n) {
        assert(n<=size_);
        data_+=n;
        size_-=n;
    }

    [[nodiscard]] std::string ToString() const
    {
        return std::string{data_,size_};
    }
private:
    const char* data_;
    std::size_t size_;
};

inline std::ostream &operator<<(std::ostream &stream, const Buf &buf) {
    return stream<<buf.data();
    
}

}
