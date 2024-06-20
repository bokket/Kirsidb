#include "write_batch_impl.h"
#include <sys/types.h>
#include <cassert>
#include <string>
#include <string_view>

#include "write_batch.h"

#include "../sstable/Buf.h"

using namespace bokket;

static const size_t kHeader=12;

WriteBatchImpl::WriteBatchImpl()
{
    clear();
}

void WriteBatchImpl::clear()
{
    bytes_.clear();
    bytes_.resize(kHeader);
}

int WriteBatchImpl::count() const
{
    return DecodeFixed32(bytes_.data()+8);
}

void WriteBatchImpl::setCount(int n)
{
    EncodeFixed32(&bytes_[8], n);
}

SequenceNumber WriteBatchImpl::Sequence()
{
    return SequenceNumber(DecodeFixed64(bytes_.data()));
}

void WriteBatchImpl::setSequence(SequenceNumber seq)
{
    EncodeFixed64(&bytes_[0], seq);
}


Status WriteBatchImpl::Iterate(WriteBatch::Handle* handler) const
{
    Buf s{bytes_};
    LOG_INFO("{}| {}",s.ToString(),s.size());
    if(s.size()<kHeader) [[unlikely]]
    {
        return Status::Corruption("malformed WriteBatch (too small)");
    }

    //12 
    s.skip(kHeader);
    LOG_INFO("{}",s.ToString());
    Buf key,value;
    int found=0;

    while(!s.empty())
    {
        found++;
        auto type=static_cast<ValueType>(s[0]);
        LOG_INFO("{}",static_cast<int>(type));
        s.skip(1);

        switch (type) {
            case ValueType::kTypePut:
                if (
                    varint::GetLengthPrefixedString(&s,&key) and 
                    varint::GetLengthPrefixedString(&s, &value)) 
                    {

                    LOG_INFO("{},{}",key.data(),value.data());
                    handler->Put(key, value);
                    }
                else
                    return Status::Corruption("Bad WriteBatch put");
                break;
            case ValueType::kTypeDelete:
                if(varint::GetLengthPrefixedString(&s, &key))
                    handler->Delete(key);
                else
                    return Status::Corruption("Bad WriteBatch delete");
                break;
            default:
                return Status::Corruption("Undefined ValueType");
        }
    }

    if(found!=count())
    {
        return Status::Corruption("WriteBatch has wrong count");
    }

    return Status::OK();
}

void WriteBatchImpl::PutRecord(const Buf& key,const Buf& value)
{
    setCount(count()+1);
    LOG_INFO("{}",bytes_.size());
    bytes_.push_back(static_cast<char>(ValueType::kTypePut));
    LOG_INFO("{}",bytes_.size());
    varint::AppendVarString(&bytes_,key);
    varint::AppendVarString(&bytes_, value);

    LOG_INFO("{}",bytes_);
}

void WriteBatchImpl::DeleteRecord(const Buf& key)
{
    setCount(count()+1);
    bytes_.push_back(static_cast<char>(ValueType::kTypeDelete));
    varint::AppendVarString(&bytes_,key);
}

std::string_view WriteBatchImpl::Contents() const
{
    return bytes_;
}

void WriteBatchImpl::SetContents(std::string_view contents)
{
    assert(contents.size()>=kHeader);

    bytes_.assign(contents.data(),contents.size());
}

void WriteBatchImpl::Append(WriteBatchImpl* dst, const WriteBatchImpl* src)
{
    setCount(dst->count()+src->count());
    dst->bytes_.append(src->bytes_.data()+kHeader,src->bytes_.size()-kHeader);
    
}