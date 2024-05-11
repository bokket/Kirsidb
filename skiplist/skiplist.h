//
// Created by bokket on 2023/12/21.
//

#pragma once


#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <random>
#include <optional>
#include <shared_mutex>


#include "../Log/Log.h"
//#include "../Arena/default.h"

namespace bokket
{

template<typename Key, typename Value>
class Node;

template<typename Key, typename Value>
class SkipList;

template<typename Key,typename Value>
using nodePtr=std::shared_ptr<Node<Key,Value>>;

template<typename Key,typename Value>
class Node {
public:
    using nodePtr=std::shared_ptr<Node<Key,Value>>;
public:
    Node() = default;

    ~Node() =default;

    explicit Node(const Key &key, const Value &value, int level)
            : key_{key}, value_{value},level_(level) {
        next_.resize(level, nullptr);
    }

    Key getKey() const {
        return this->key_;
    }

    Value getValue() const {
        return this->value_;
    }

    // [[nodiscard]] size_t getLevel() const {
    //     return this->level_;
    // }

    void Unlink() {
        for (auto &p: next_) {
            p = nullptr;
        }
    }

    [[nodiscard]] std::size_t getSize() const {
        return this->next_.size();
    }
public:
    const Key key_;
    Value value_;

    int level_;

    std::vector<nodePtr> next_;
    //std::vector<Node *> next_;
};



template<typename Key,typename Value>
class SkipList
{
public:
    //explicit SkipList(std::shared_ptr<DefaultAlloc> alloc);
    //explicit SkipList(size_t list_level);
    SkipList();

    void Insert(const Key &k,const Value & v);

    void Delete(const Key& k);

    bool Contains(const Key& k);

    std::optional<Value> Get (const Key& k);

    void Print();

    size_t getMaxLevel() const {
        return max_level_;
    }

    size_t size() const {
        return elementCount_;
    }

    void setHeightIncrease(size_t increase) {
        kBranching=increase;
    }

    std::optional<size_t> getHeightIncrease() {
        return kBranching;
    }


    class SkipListIterator {
    public:
        explicit SkipListIterator(const SkipList* list);

        bool Valid();

        const Key &key();

        const Value &value();

        void Next();

        void Prev()=delete;

        void MoveToFirst();

        void MoveToLevel(size_t level);

        std::size_t Size();

    private:
        const SkipList* list_;

        std::optional<nodePtr<Key,Value>> node_;
    };

private:
    inline nodePtr<Key,Value> From(const Key&,const Value&,size_t level);

    //std::vector<nodePtr<Key,Value>> FindPrevNode(const Key &key, std::vector<nodePtr<Key,Value>> &prev);

     std::vector<nodePtr<Key,Value>> FindPrevNode(const Key &key);
    //void FindPrevNode(const Key &key,std::vector<nodePtr<Key,Value>>& prev);

    size_t getRandomLevel();

private:

    constexpr static size_t  kMaxLevelNum=12;

    size_t kBranching = 12;

    size_t max_level_;

    std::atomic<size_t> elementCount_;

    mutable std::shared_mutex mutex_;

    nodePtr<Key,Value> head_;

//    std::shared_ptr<DefaultAlloc> alloc_;

    //std::shared_ptr<lo>
    //Node* head_;
};



}


