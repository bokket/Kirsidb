#pragma once

#include <mutex>
#include <functional>
#include <string_view>

#include "skiplist.h"

#include "internalKeys.h"

#include "../sstable/Buf.h"

#include "../iterator/iterator.h"


// #include "../sstable/coding.h"

namespace bokket 
{

class MemTableIterator;

class InternalKeyComparator;

class Memtable
{
    private:
        friend class MemTableIterator;

    private:
        using Compare=std::function<int(const char*,const char*)>;
        using Table=SkipList<const char*, Compare>;

        Table table_;
        std::mutex mutex_;
        InternalKeyComparator comparator_;
    public:
        explicit Memtable(const InternalKeyComparator& comparator);    

        void Add(SequenceNumber seq,ValueType type,const Buf& key,const Buf& value);

    public:
        using Entry=std::pair<std::string_view,std::string_view>;

        class SkipListIterator;

        using Iter=SkipListIterator;

        [[nodiscard]] Iter begin() const;

        [[nodiscard]] Iter end() const;   

        bool find(const LookupKey& key,std::string* value) const;

        Iterator* NewIterator();
};

class MemTableIterator:public Iterator
{
    private:

        Memtable::Table::Iter iter_;
        std::string tmp_;
    public:
        explicit MemTableIterator(Memtable::Table* table)
                                 :iter_{table}
        {}

        void Seek(const Buf& target) override;

        void SeekToFrist() override;

        void Next() override
        {
            iter_.Next();
        }

        [[nodiscard]] bool Valid() const override
        {
            return iter_.Valid();
        }

        [[nodiscard]] std::string key() const override;

        [[nodiscard]] std::string value() const override;

        [[nodiscard]] Status status() const override
        {
            return Status::OK();
        }
    
};


class Memtable::SkipListIterator:public IteratorFacade<SkipListIterator, Entry, ForwardIteratorTag, true>
{
    friend class Memtable;

    public:
        std::string_view key() const;

        std::string_view value() const;

        bool valid() const;

        bool equal(const Iter& other) const; 
    private:
        Table::Iter iter_;
        mutable Entry entry_;

    private:
        explicit SkipListIterator(const Table::Iter &iter)
                                 :iter_{iter}
        {
            update();
        }

        void update() const;

        void increment();

        // bool equal(const Iter& other) const; 
};


}