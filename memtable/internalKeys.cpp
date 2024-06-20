#include "internalKeys.h"

using namespace bokket;

// seq = 1
// type = 255
// 0000 0001
// 0000 0001 0000 0000 (256)
// 0000 0000 1111 1111 (255)
// 0000 0001 1111 1111 (511)
static inline uint64_t packSequenceAndType(SequenceNumber seq,ValueType type)
{
    assert(type<=kTypeForSeek);
    return static_cast<uint64_t>( (seq<<8) | static_cast<uint8_t>(type));
}


void AppendInternalKey(std::string *result, const class ParsedInternalKey &key)
{
    result->append(key.user_key_.data(),key.user_key_.size());

    PutFixed64(*result, packSequenceAndType(key.seq_, key.type_));
}

std::string EscapeString(std::string_view value)
{
    std::string r;
    for(char c : value)
    {
        if(c>=' ' && c<='~')
        {
            r.push_back(c);
        }
        else{
            r += fmt::format("\\x{:02x}", static_cast<unsigned int>(c) & 0xff);
        }
    }
    return r;
}

std::string ParsedInternalKey::DebugString() const
{
    std::ostringstream ss;

    ss<<'\''<<EscapeString(user_key_)<<"' @"<<seq_<<" : "
    <<static_cast<int>(type_)<<" ("<<ValueTypeToString(type_)<<")";

    return ss.str(); 
}


int InternalKeyComparator::Compare(std::string_view a, std::string_view b) const
{

    auto r=user_comparator_->Compare(ExtractUserKey(a),ExtractUserKey(b));

    if(r==0)
    {
        const uint64_t anum=DecodeFixed64(a.data()+a.size()-8);
        const uint64_t bnum=DecodeFixed64(b.data()+b.size()-8);

        if(anum<bnum)
        {
            r=-1;
        } else if(anum>bnum)
        {
            r=+1;
        }
    }

    return r;
}


/*
            encode(internal_key_size)   key   sequence(7B)type(1B)  encode(value_size)  value
            |                       |         |                 |
            |                       |<userkey>|<------tag------>|
            |                       |                           |
            |                       |<-------internal key------>|
            |                  memtable key                     |
            |<----------------------|-------------------------->|
            |                       |                           |
LookupKey:start_                  kstart_                      end_
*/


LookupKey::LookupKey(std::string_view user_key,SequenceNumber seq)
{
    auto user_size=user_key.size();

    auto needed=user_size+13;//(8+5)

    char* dst;
    if(needed<=sizeof(space_))
        dst=space_;
    else 
        dst=new char[needed];


    start_=dst;

    dst=varint::EncodeVarint32(dst,user_size+8);
    kstart_=dst;


    std::memcpy(dst,user_key.data(),user_size);
    dst+=user_size;

    EncodeFixed64(dst, packSequenceAndType(seq, kTypeForSeek));

    dst+=8;
    end_=dst;
}

LookupKey::~LookupKey()
{
    if(start_!=space_)
        delete [] start_;
}

std::string_view LookupKey::memtable_key() const
{
    return std::string_view{start_,static_cast<std::string_view::size_type>(end_-start_)};
}

std::string_view LookupKey::internal_key() const
{
    return std::string_view{kstart_,static_cast<std::string_view::size_type>(end_-kstart_)};
}

std::string_view LookupKey::user_key() const
{
    return std::string_view{kstart_,static_cast<std::string_view::size_type>(end_-kstart_-8)};
}

