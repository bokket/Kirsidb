//
// Created by bokket on 24-3-27.
//

#pragma once

#include <list>

#include <any>
#include <string>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <cassert>

#include "../Log/Log.h"

namespace bokket
{


class LRUCachePolicy;
//template<typename K,typename V>
//class LRUCachePolicy<K,V> {
class LRUCachePolicy {
public:
    LRUCachePolicy(size_t capacity)
                 :capacity_{capacity}
    {}

    ~LRUCachePolicy()=default;

    static LRUCachePolicy *Default(size_t capacity);
public:
    using CacheList=std::list<std::string>;

    struct LRUHandle {
        std::any value_;
        CacheList::const_iterator pos_;
    };

    using CacheTable=std::unordered_map<std::string_view ,LRUHandle>;
public:
    LRUHandle* Insert(std::string_view key,const std::any& value);

    void Erase(std::string_view key);

    LRUHandle* Lookup(std::string_view key);

    std::any &Value(LRUHandle* handle) const;

    uint64_t NewId();


private:
    CacheTable table_;
    CacheList cache_;
    const size_t capacity_;
    uint64_t unique_id_;
    mutable std::mutex mutex_;
};

}


