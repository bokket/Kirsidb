#include "memtable.h"
#include <memory>
#include <string>
#include <string_view>

#include "skiplist.cpp"

using namespace bokket;

static inline std::string GetVarString(const char* s)
{
    uint32_t len;
    auto p=s;
    p=varint::GetVarint32Ptr(p,p+5,&len);

    return std::string{p,len};   
}

Memtable::Memtable(const InternalKeyComparator& comparator)
                  :comparator_{comparator}
                  ,table_([this](const char* a_buf,const char* b_buf) -> int{
                        auto a=GetVarString(a_buf);
                        auto b=GetVarString(b_buf);
                        return this->comparator_.Compare(a,b);
                  })
{}

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
void Memtable::Add(SequenceNumber seq,ValueType type,const Buf& key,const Buf& value)
{
    auto key_size=key.size();
    auto val_size=value.size();

    auto internal_key_size=key_size+8;

    const auto encoded_len=varint::VarintLength(internal_key_size)
                                        +internal_key_size+varint::VarintLength(val_size)
                                        +val_size;

    LOG_INFO("{}",encoded_len);

    auto p_buf=std::make_unique<char[]>(encoded_len);
    //std::unique_ptr<char[]> p_buf(new char[encoded_len]);
    //auto p_buf=std::make_shared<char[]>(encoded_len);

    auto buf=p_buf.get();
    //auto buf=new char[encoded_len];

    char* p=varint::EncodeVarint32(buf,internal_key_size);

    std::memcpy(p,key.data(),key_size);

    p+=key_size;

    auto typeValue=static_cast<uint64_t>(type);
    EncodeFixed64(p, (seq<<8)|typeValue);

    p+=8;

    auto tmp=p;

    p=varint::EncodeVarint32(p, val_size);

    LOG_INFO("{}",value.data());
    std::memcpy(p, value.data(), val_size);

    assert(p+val_size==buf+encoded_len);

    LOG_INFO("{}",std::string{tmp,varint::VarintLength(val_size)
                                        +val_size});

    LOG_INFO("{},{}",std::string{buf,encoded_len},std::string{p,val_size});

    LOG_INFO("{}",std::string{buf,encoded_len});

    // std::string res{buf,encoded_len};

    auto iter=table_.Insert(buf);

    LOG_INFO("{}",iter.key());

    p_buf.release();
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
bool Memtable::find(const LookupKey& key,std::string* value) const
{
    auto memkey=key.memtable_key();

    LOG_INFO("{}",memkey.data());

    auto iter=table_.Find(memkey.data());

    LOG_INFO("{}",iter.key());
    
    if(iter.Valid())
    {
        auto entry=iter.key();

        uint32_t key_length;
        auto key_ptr=varint::GetVarint32Ptr(entry,entry+5,&key_length);

        if(comparator_.user_comparator()->Compare(std::string(key_ptr,key_length-8), key.user_key())==0)
        {
            const uint64_t tag=DecodeFixed64(key_ptr+key_length-8);

            switch (static_cast<ValueType>(tag&0xff)) {
                case ValueType::kTypePut:
                {
                    auto v=GetVarString(key_ptr+key_length);
                    value->assign(v.data(),v.size());
                    return true;
                }
                case ValueType::kTypeDelete:
                {
                    LOG_INFO("delet");
                    return true;
                }
            }
        }
        
    }
    return false;
}



Memtable::Iter Memtable::begin() const
{
    return Memtable::Iter{table_.Begin()};
}

Memtable::Iter Memtable::end() const
{
    return Memtable::Iter{table_.End()};
}


std::string_view Memtable::Iter::key() const
{
    update();
    return entry_.first;
}

std::string_view Memtable::Iter::value() const
{
    update();
    //return 
}


void Memtable::Iter::update() const
{

}

void Memtable::Iter::increment() 
{
}



bool Memtable::Iter::equal(const Iter& other) const
{
    return iter_==other.iter_;
}


bool Memtable::Iter::valid() const
{
    return iter_.Valid();
}


Iterator* Memtable::NewIterator()
{
    return new MemTableIterator(&table_);
}


std::string MemTableIterator::key() const
{
    return GetVarString(iter_.key());
}

std::string MemTableIterator::value() const
{
    auto entry=iter_.key();

    uint32_t key_length;
    auto key_ptr=varint::GetVarint32Ptr(entry,entry+5,&key_length);

    auto v=GetVarString(key_ptr+key_length);
    return v;

    // auto key=GetVarString(iter_.key());
    // LOG_INFO("{} | {}",key,iter_.key());
    // LOG_INFO("{}",GetVarString(key.data()+key.size()));
    // return GetVarString(key.data()+key.size());
}

static const char* EncodeKey(std::string* dst,const Buf& target)
{
    dst->clear();
    varint::AppendVar32(dst, target.size());

    dst->append(target.data(),target.size());

    return dst->data();
}

void MemTableIterator::Seek(const Buf& target)
{
    return iter_.Seek(EncodeKey(&tmp_, target));
}

void MemTableIterator::SeekToFrist()
{
    iter_.SeekToFirst();
}