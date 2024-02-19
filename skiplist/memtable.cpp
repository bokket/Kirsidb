//
// Created by bokket on 2024/1/29.
//

#include "memtable.h"

using namespace bokket;


template<typename K,typename V>
Memtable<K,V>::Memtable()
{
    table_=std::make_shared<SkipList<K,V>>(12);
}

template<typename K,typename V>
void Memtable<K, V>::Insert() {

}

template<typename K,typename V>
Memtable<K,V>::


template<typename K,typename V>
MemtableIterator<K,V>::MemtableIterator(SkipList<K, V> *list)
                                        :iter_{std::make_shared<Iter>(list)}
{}

template<typename K,typename V>
void MemtableIterator<K, V>::MoveToFirst() {
    iter_->MoveToFirst();
}

template<typename K,typename V>
const V &MemtableIterator<K, V>::value() {
    iter_->value();
}

template<typename K,typename V>
const K &MemtableIterator<K, V>::key() {
    return iter_->key();
}

template<typename K,typename V>
void MemtableIterator<K, V>::Next() {
    return iter_->Next();
}

template<typename K,typename V>
bool MemtableIterator<K, V>::Valid() {
    return iter_->Valid();
}