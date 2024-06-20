#include "./write_batch.h"
#include <cassert>
#include <string_view>

#include "write_batch_impl.h"

using namespace bokket;

WriteBatch::WriteBatch()
                      :impl_(std::make_unique<WriteBatchImpl>())
{}

WriteBatch::~WriteBatch()=default;

void WriteBatch::Clear()
{
    impl_->clear();
}

void WriteBatch::SetCount(int n)
{
    impl_->setCount(n);
}

int WriteBatch::Count() const
{
    return impl_->count();
}

void WriteBatch::setSequence(SequenceNumber seq)
{
    return impl_->setSequence(seq);
}

SequenceNumber WriteBatch::Sequence()
{
    return impl_->Sequence();
}

void WriteBatch::Put(const Buf &key, const Buf &value)
{
    impl_->PutRecord(key, value);
}

void WriteBatch::Delete(const Buf & key)
{
    impl_->DeleteRecord(key);
}

std::string_view WriteBatch::Contents() const
{
    return impl_->Contents();
}

void WriteBatch::SetContents(std::string_view contents)
{
    impl_->SetContents(contents);
}

void WriteBatch::Append(const WriteBatch& source)
{
    //SetCount(this->Count()+source.Count());
    impl_->Append(this->impl_.get(),source.impl_.get());
}

Status WriteBatch::InsertInto(Memtable* table)
{
    LOG_INFO("{}",Sequence());
    //assert(Sequence());
    MemtableInserter inserter{table,Sequence()};

    LOG_INFO("{}",Sequence());

    return impl_->Iterate(&inserter);
}

Status WriteBatch::Iterate(Handle* handler) const
{
    return impl_->Iterate(handler);
}