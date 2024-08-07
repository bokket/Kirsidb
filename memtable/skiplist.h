#pragma once


#include <sys/types.h>
#include <cassert>
#include <cstddef>
#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <random>
#include <iostream>
#include <iomanip>
#include <optional>
#include <shared_mutex>

#include "../Log/Log.h"
#include "../iterator/iterator_trait.h"

namespace bokket
{

template <typename Key,class Comparator>
class SkipList
{
public:
    class Node;


    using nodePtr=std::shared_ptr<Node>;
public:
    explicit SkipList(Comparator cmp);
    //~SkipList();
public:
    struct SkipListIterator;
public:
    using SkipListIteratorFacade=
    IteratorFacade<SkipListIterator, Key, ForwardIteratorTag, true>;

    struct SkipListIterator:public SkipListIteratorFacade
    {
        public:
            explicit SkipListIterator(const SkipList* list)
                            :list_{list}
                            ,node_{nullptr}
            {
                // if(list_!=nullptr)
                // {
                //     LOG_INFO("list! nullptr");
                //     node_=list_->head_->next_[0];
                // }
            }

            // explicit SkipListIterator(Node* next=nullptr)
            //                         :list_{nullptr}
            //                         ,node_{next}
            // {}

            explicit SkipListIterator(const SkipList* list,Node* node)
                                     :list_{list}
                                     ,node_{node}
            {

            }

            explicit SkipListIterator(const SkipList* list,nodePtr node)
                                     :list_{list}
                                     ,node_{std::move(node)}
            {

            }

            // ~SkipListIterator()
            // {
            //     if(node_!=nullptr)
            //     {
            //         LOG_INFO("?");
            //         delete node_;
            //     }
            // }

            SkipListIterator(const SkipListIterator& other)
            {
                list_=other.list_;
                node_=other.node_;
            }

            [[nodiscard]] bool Valid() const {
                return node_!=nullptr;
            }

            const Key& key() const 
            {
                assert(Valid());
                return node_->getKey();
            }

            bool equal(const SkipListIterator& other) const {
                return node_==other.node_;
            }

            void Next()
            {
                assert(Valid());
                node_=node_->next_[0];
            }

            void Seek(const Key& target)
            {
                auto iter=list_->Find(target);
                assert(Valid());
                node_=iter.node_;

                //LOG_INFO("{}",node_->getKey());
            }

            void SeekToFirst() 
            {
                node_=list_->head_->next_[0];
                Valid();
            }

        private:
            void increment() {
                assert(Valid());
                node_=node_->next_[0];
            }

        private:
            const SkipList* list_;
            //Node* node_;
            nodePtr node_;
    };

public:
    using Iter=SkipListIterator;

    Iter Insert(const Key& key);

    //void Insert(const Key& key);

    bool Contains(const Key& key) const;

    Iter Find(const Key& target) const;

    Iter Begin() const;

    Iter End() const;

    void Print();

private:
    [[nodiscard]] inline int GetMaxHeight() const;

    std::vector<nodePtr> FindPrevNode(const Key &key);
    //std::vector<Node*> FindPrevNode(const Key &key);

    //inline Node* From(const Key&,int level);
    inline nodePtr From(const Key&,int level);


    inline int getRandomLevel();
private:
    constexpr static int kMaxLevelNum=12;

    //Node* const head_;
    nodePtr head_;


    const Comparator cmp_;

    mutable std::shared_mutex mutex_;

    std::atomic<int> max_level_;
};


template<typename Key,class Comparator>
class SkipList<Key,Comparator>::Node
{
public:    
    using nodePtr=std::shared_ptr<Node>;
    const Key key_;
    //Value value_;


    //std::vector<Node*> next_;
    std::vector<nodePtr> next_;

    explicit Node(const Key& k,int level)
//    explicit Node(const Key& k,const Value& v,int level)
                 :key_{k}
//                 ,value_{v}
    {
        next_.resize(level,nullptr);
    }

    // ~Node()
    // {
    //     for (auto &p: next_) {
    //         if(p!=nullptr)
    //         {
    //             LOG_INFO("?");
    //             delete p;
    //         }
    //     }
    // }

    inline int getLevel()
    {
        return next_.size();
    }

    const Key& getKey() const {
        return key_;
    }
};



}