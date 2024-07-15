//
// Created by bokket on 24-3-27.
//

#include "cache.h"

#include <utility>

using namespace bokket;

LRUCachePolicy::LRUHandle *LRUCachePolicy::Insert(std::string_view key, const std::any &value) {
    std::lock_guard<std::mutex> lockGuard{mutex_};

    assert(cache_.size()<=capacity_);
    assert(table_.size()==cache_.size());

    auto it=table_.find(key.data());
    if(it!=table_.end()) {
        LOG_INFO("insert:{},{} | cache:{}",it->first,it->second.pos_->data(),cache_.size());
        cache_.erase(it->second.pos_);
        table_.erase(it);
    } else if(capacity_==cache_.size()) {
        LOG_INFO("erase:{},{}",cache_.rbegin()->data(),table_[cache_.rbegin()->data()].pos_->data());

        table_.erase(*cache_.rbegin());
        cache_.erase(std::prev(cache_.end()));
        //cache_.erase(cache_.rbegin());
    }

    //emplace方法在unordered_map容器中插入一个新元素，如果元素已存在，则不会插入新元素。它返回一个 std::pair
    //，其中第一个元素是一个指向新元素或现有元素的迭代器，第二个元素是一个布尔值，指示是否插入了新元素。
    it=table_.emplace(std::make_pair<std::string ,LRUHandle>(
            key.data(),{key.data(),value}
            )).first;



    cache_.emplace_front(it->first);
    it->second.pos_=cache_.begin();

    //it->second.deleter=deleter;


    LOG_INFO("insert:{},{}",it->first.data(),it->second.key_);

    return reinterpret_cast<LRUHandle*>(&it->second);
}

void LRUCachePolicy::Release(LRUHandle* handle)
{
    std::lock_guard<std::mutex> lockGuard{mutex_};

    //handle->deleter(handle->key_,handle->value_);
}

void LRUCachePolicy::Erase(std::string_view key) {
    std::lock_guard<std::mutex> lockGuard{mutex_};

    auto it=table_.find(key.data());
    if(it!=table_.end()) {
        cache_.erase(it->second.pos_);
        table_.erase(it);
    }
}

LRUCachePolicy::LRUHandle* LRUCachePolicy::Lookup(std::string_view key) {
    std::lock_guard<std::mutex> lockGuard{mutex_};
    auto it= table_.find(key.data());
    if(it==table_.end())
        return nullptr;

    cache_.erase(it->second.pos_);

    auto LRUhandle=std::move(it->second);

    table_.erase(it);

    it=table_.emplace(
        std::make_pair(key.data(),std::move(LRUhandle))
    ).first;

    cache_.emplace_front(it->first);

    //LOG_INFO("{},{}",it->second.pos_->data(),cache_.begin()->data());
    it->second.pos_=cache_.begin();

    //LOG_INFO("{},{}",it->second.pos_->data(),cache_.begin()->data());

    return reinterpret_cast<LRUHandle*>(&it->second);
}

std::any &LRUCachePolicy::Value(LRUHandle* handle) const {
    return handle->value_;
}

uint64_t LRUCachePolicy::NewId() {
    std::lock_guard<std::mutex> lockGuard{mutex_};
    return unique_id_++;
}