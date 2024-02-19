//
// Created by bokket on 2024/1/29.
//

#pragma once


#include <memory>

#include "skiplist.h"
#include "../base/noncopyable.h"

namespace bokket
{
    template<typename K,typename V>
    class Memtable;

    template<typename K,typename V>
    class MemtableIterator;


    template<typename K,typename V>
class Memtable:noncopyable
{
public:
    explicit Memtable();

    ~Memtable()=default;

    void Add();

private:
    void Insert();


private:
    std::shared_ptr<SkipList<K,V>> table_;


};


template<typename K,typename V>
class MemtableIterator
{
private:
    using Iter=typename SkipList<K,V>::SkipListIterator;
    std::shared_ptr<Iter> iter_;
public:
    explicit MemtableIterator(SkipList<K,V>* list);

    void MoveToFirst();

    void Next();

    const K & key();

    const V & value();

    bool Valid();

};

}
