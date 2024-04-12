//
// Created by bokket on 24-3-27.
//

#include "cache.h"

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

        table_.erase(cache_.rbegin()->data());
        cache_.erase(std::prev(cache_.end()));
        //cache_.erase(cache_.rbegin());
    }

    it=table_.emplace(std::make_pair<std::string_view ,LRUHandle>(
            key.data(),{value}
            )).first;



    cache_.emplace_front(it->first);
    it->second.pos_=cache_.begin();

    LOG_INFO("insert:{},{}",it->first.data(),it->second.pos_->data());

    return reinterpret_cast<LRUHandle*>(&it->second);
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

    LOG_INFO("lookup:{},{}",LRUhandle.pos_->data(), std::any_cast<int>(Value(&LRUhandle)));

    table_.erase(it);

    it=table_.emplace(
        std::make_pair(key.data(),std::move(LRUhandle))
    ).first;

    cache_.emplace_front(it->first);

    LOG_INFO("{},{}",it->second.pos_->data(),cache_.begin()->data());
    it->second.pos_=cache_.begin();

    LOG_INFO("{},{}",it->second.pos_->data(),cache_.begin()->data());

    return reinterpret_cast<LRUHandle*>(&it->second);
}

std::any &LRUCachePolicy::Value(LRUHandle* handle) const {
    return std::any_cast<LRUHandle*>(handle)->value_;
    //return handle->value_;
}

uint64_t LRUCachePolicy::NewId() {
    std::lock_guard<std::mutex> lockGuard{mutex_};
    return unique_id_++;
}