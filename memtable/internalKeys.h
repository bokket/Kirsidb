#pragma once

#include <cstring>
#include <cassert>
#include <cstdint>

#include <string>
#include <string_view>
#include <sstream>

#include <fmt/format.h>

#include "comparator.h"

#include "../sstable/coding.h"

namespace bokket
{

enum class ValueType
{
    kTypeDelete=0x0,
    kTypePut=0x1
};

static const ValueType kTypeForSeek=ValueType::kTypePut;


using SequenceNumber=std::uint64_t;

inline std::string ValueTypeToString(ValueType type)
{
    switch (type) {
        case ValueType::kTypeDelete:
            return "kTypeDelete";
            break;
        case bokket::ValueType::kTypePut:
            return "kTypePut";
            break;
        default:
            return "No Value Type";
            break;
    }
}

class ParsedInternalKey
{
    public:
        std::string user_key_;
        SequenceNumber seq_{};
        ValueType type_{};

    public:
        ParsedInternalKey()=default;

        ParsedInternalKey(std::string_view str,const SequenceNumber& seq,ValueType type)
                   :user_key_{str}
                   ,seq_{seq}
                   ,type_{type}
        {}

        [[nodiscard]] std::string DebugString() const;
};




void AppendInternalKey(std::string* result,const ParsedInternalKey& key);

inline void AppendNumber(std::string* str,uint64_t num)
{
    str->append(fmt::format("{}",num));
    // char buf[30];
    // std::snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(num));
    // str->append(buf);
}

inline std::string NumberToString(uint64_t num)
{
    std::string r;
    AppendNumber(&r,num);
    return r;
}


inline bool Parsed(std::string_view internal_key,ParsedInternalKey* result)
{
    const auto n=internal_key.size();

    if(n<8)
        return false;

    uint64_t num=DecodeFixed64(internal_key.data()+n-8);

    uint8_t c=num & 0xff;
    
    result->seq_=num>>8;


    result->type_=static_cast<ValueType>(c);

    result->user_key_=std::string_view{internal_key.data(),n-8};

    return (c<=static_cast<uint8_t>(ValueType::kTypePut));
}



inline std::string ExtractUserKey(std::string_view internal_key)
{
    assert(internal_key.size()>8);

    return std::string{internal_key.data(),internal_key.size()-8};
}

class InternalKey
{
    private:
        std::string rep_;
    public:
        InternalKey()=default;

        InternalKey(std::string_view user_key,SequenceNumber seq,ValueType type);

        [[nodiscard]] std::string_view Data() const {
            assert(!rep_.empty());
            return rep_;
        }

        
};

class InternalKeyComparator:public Comparator
{
    private:
        const Comparator* user_comparator_;
    public:
        explicit InternalKeyComparator(const Comparator* cmp)
                                      :user_comparator_{cmp}
        {}

        [[nodiscard]] int Compare(std::string_view a, std::string_view b) const override;


        [[nodiscard]] const Comparator* user_comparator() const 
        {
            return user_comparator_;
        }

        [[nodiscard]] inline int Compare(const InternalKey& a,const InternalKey& b) const
        {
            return Compare(a.Data(),b.Data());
        }
};

class LookupKey
{
    private:
        const char* start_;
        const char* kstart_;
        const char* end_;
        char space_[200];
    public:
        LookupKey(std::string_view user_key,SequenceNumber seq);

        ~LookupKey();

        [[nodiscard]] std::string_view memtable_key() const;

        [[nodiscard]] std::string_view internal_key() const;

        [[nodiscard]] std::string_view user_key() const;
};

}
