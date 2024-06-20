#pragma once

#include <memory>
#include <string>
#include <string_view>


#include "./write_batch.h"

namespace bokket
{

class WriteBatchImpl
{
    private:
        std::string bytes_;

    public:
        WriteBatchImpl();

        void clear();

        [[nodiscard]] int count() const;

        // static int Count(const WriteBatch* batch);

        void setCount(int n);

        [[nodiscard]] std::string_view Contents() const;

        void SetContents(std::string_view contents);

        SequenceNumber Sequence();

        void setSequence(SequenceNumber seq);

        void Append(WriteBatchImpl* dst, const WriteBatchImpl* src);

        Status Iterate(WriteBatch::Handle* handler) const;

        //Status InsertInto(const WriteBatch* batch,Memtable* memtable);

        void PutRecord(const Buf& key,const Buf& value);

        void DeleteRecord(const Buf& key);
};

}