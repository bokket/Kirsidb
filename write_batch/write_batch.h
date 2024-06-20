#pragma once

#include <memory>
#include <string_view>

#include "../sstable/Buf.h"
#include "../db/status.h"


#include "../memtable/memtable.h"
#include "../memtable/internalKeys.h"

namespace bokket 
{


class Memtable;
class WriteBatchImpl;


class WriteBatch
{
    private:
        std::unique_ptr<WriteBatchImpl> impl_;

    public:
        WriteBatch();

        ~WriteBatch();

        void Put(const Buf &key, const Buf &value);

        void Delete(const Buf & key);

        void Clear();

        void SetCount(int n);

        [[nodiscard]] int Count() const;


        void setSequence(SequenceNumber seq);

        SequenceNumber Sequence();

        [[nodiscard]] std::string_view Contents() const;
        
        void SetContents(std::string_view contents);

        void Append(const WriteBatch& source);

        class Handle
        {
            public:
                virtual ~Handle()=default;

                virtual void Put(const Buf& key,const Buf& value)=0;

                virtual void Delete(const Buf& key)=0;
        };

        Status Iterate(Handle* handler) const;

        Status InsertInto(Memtable* table);
};


class MemtableInserter:public WriteBatch::Handle
{
    public:
        explicit MemtableInserter(Memtable* table,SequenceNumber seq)
                                :table_{table}
                                ,seq_{seq}
        {}

        void Put(const Buf& key,const Buf& value) override
        {
            LOG_INFO("{}| {} | {}",NumberToString(seq_),key.data(),value.data());
            table_->Add(seq_, bokket::ValueType::kTypePut, key, value);
            ++seq_;
        }


        void Delete(const Buf& key) override
        {
            table_->Add(seq_, bokket::ValueType::kTypeDelete, key,Buf());
            ++seq_;
        }

    private:
        SequenceNumber seq_;
        Memtable* table_;
};

}