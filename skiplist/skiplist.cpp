//
// Created by bokket on 2023/12/28.
//

#include "skiplist.h"

using namespace bokket;

//auto logger=Logger::getInstance()->init("../logs/skiplist.log");


template<typename Key,typename Value>
SkipList<Key,Value>::SkipList(size_t list_level)
                             :max_level_(list_level)
                             ,elementCount_(0)
//SkipList<Key,Value>::SkipList(std::shared_ptr<DefaultAlloc> alloc)
//                             :elementCount_(0)
//                             ,alloc_(std::move(alloc))
{
    static_assert(std::is_default_constructible<Key>(),"The Key type must has default constructor");
    static_assert(std::is_default_constructible<Value>(), "The Key type must has default constructor");
    Key k;
    Value v;
    LOG_INFO("skiplist head k:{},v:{}",k,v);
//    head_ = From(k, v, max_level_={getHeightIncrease().value()});
//    static_assert(max_level_==getHeightIncrease().value(),"SkipList max level Must EQ!");
    head_= From(k,v,max_level_);
//    std::cout<<head_.get()<< std::endl;
    LOG_INFO("SkipList level height is max_level {}\n "
             "getHeightIncrease() is {}",max_level_,getHeightIncrease().value());

}


template<typename Key,typename Value>
nodePtr<Key,Value> SkipList<Key,Value>::From(const Key& k,const Value& v,size_t level) {
    LOG_INFO("skiplist new node k:{},v:{}",k,v);
    return nodePtr<Key,Value>(new Node<Key,Value>(k,v,level));
}


/*
cur                             +------------+
 |                              |  select 60 |
 |                              +------------+
head--->level 4     +-->1+                                                      100
                         |
                         |cur
        level 3         1+-------->10+------------------>50+           70       100
                                                           |
                                                           |
        level 2         1          10         30         50|           70       100
                                                           |
                                                           |
        level 1         1    4     10         30         50|           70       100
cur                                                        |
 |                                                         |
head    level 0         1    4   9 10         30   40    50+-->60      70       100
*/

template<typename Key,typename Value>
std::optional<Value> SkipList<Key, Value>::Get(const Key &k) {
    std::shared_lock<std::shared_mutex> sl{mutex_};
    auto cur= this->head_;

    for(ssize_t i= this->max_level_-1;i>=0;i--){

        while(cur->next_[i]!= nullptr and cur->next_[i]->key_ < k) {
            cur=cur->next_[i];
        }
        if(cur->next_[i]!= nullptr and cur->next_[i]->key_ == k) {
            LOG_INFO("Get value:{} | key:{}",cur->next_[i]->getValue(),cur->next_[i]->getKey());
            //std::cout<<cur->next_[i]->getLevel()<<"| "<<cur->next_[i]->getValue()<<std::endl;
            return cur->next_[i]->value_;
        }
//        else
//            return std::nullopt;
    }
    return std::nullopt;
}

template<typename Key,typename Value>
bool SkipList<Key, Value>::Contains(const Key &k) {
    return Get(k).has_value();
}

//template<typename Key,typename Value>
//std::vector<nodePtr<Key, Value>>
//SkipList<Key, Value>::FindPrevNode(const Key &key, std::vector<nodePtr<Key, Value>> &prev) {
//
//}

template<typename Key,typename Value>
std::vector<nodePtr<Key, Value>> SkipList<Key, Value>::FindPrevNode(const Key &key) {
//void SkipList<Key, Value>::FindPrevNode(const Key &key, std::vector<nodePtr<Key, Value>> &prev)  {
    std::vector<nodePtr<Key,Value>> prev(this->max_level_, nullptr);
    //printf("%s\n",prev);

    LOG_INFO("max:{} | head_:{} | k:{}",max_level_,head_->getValue(),key);

    LOG_INFO("here{}",head_->next_.size());
    nodePtr<Key,Value> cur=this->head_;
    for (std::size_t i=max_level_-1; i-- > 0;) {


        while(cur->next_[i]!= nullptr and cur->next_[i]->key_<key) {
            cur=cur->next_[i];
        }

        prev[i]=cur;
        //std::cout<<prev[i].get()<< std::endl;

    }
    //std::cout<<prev[0].get()<< std::endl;
    return prev;
}


// Insert given key and value in skip list
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+
*/
//TODO!! Is Need return?
template<typename Key,typename Value>
void SkipList<Key, Value>::Insert(const Key &k, const Value &v) {

    if(Contains(k)) {
        LOG_WARN("Key has been inserted,Key={}",k);
        return;
    }

    std::unique_lock<std::shared_mutex> ul{mutex_};

    ++elementCount_;
    LOG_INFO("SkipList element count increase ,now count is {}",elementCount_);

    auto lv=getRandomLevel();
//    LOG_INFO("lv:{}",lv);
    //max_level_=std::max(lv,max_level_);
    auto newNode= From(k,v,lv);

    //TODO!! 优化
    auto pre= FindPrevNode(k);
//    std::vector<nodePtr<Key,Value>> pre{max_level_, nullptr};
//    FindPrevNode(k,pre);
    //std::cout<<newNode->getLevel()<< std::endl;

    for (std::size_t i=newNode->getLevel(); i-- > 0;) {
        if(pre[i]== nullptr) {
            newNode->next_[i]= nullptr;
            head_->next_[i]=newNode;
        }
        newNode->next_[i]=pre[i]->next_[i];
        pre[i]->next_[i]=newNode;
    }

    //Print();
}

//TODO Need trun?
template<typename Key,typename Value>
void SkipList<Key, Value>::Delete(const Key &k) {
    if (!Contains(k)) {
        LOG_ERROR("The value you want to delete does not exist. Key={} ",k);
        return;
    }

    std::unique_lock<std::shared_mutex> ul{mutex_};

    auto pre= FindPrevNode(k);

    auto eraseNode=pre[0]->next_[0];
//    static_assert(eraseNode->getKey()==k,"SkipList Delete node key value must EQ!");

    LOG_INFO("SkipList Delete node key:{} value:{}",eraseNode->key_,eraseNode->value_);

    for(auto i=0;i<pre.size();++i) {
        if(pre[i]->next_[i]!=eraseNode)
            break;
        pre[i]->next_[i]=eraseNode->next_[i];
    }
    --elementCount_;
    return;
}

template<typename Key,typename Value>
size_t SkipList<Key, Value>::getRandomLevel() {
    //static_assert(max_level_>12,"SkipList Level num default 12");

    //马特赛特旋转演算法
    //static std::mt19937 mt_rand(0);
//    static std::mt19937 mt_rand(std::random_device{}());
//
//
//    size_t k = 1;
//
//    std::cout<<getHeightIncrease().value()<<std::endl;
//
//    auto rand=mt_rand()% getHeightIncrease().value();
//
//    LOG_INFO("{}",rand);
//
//    while (k < kMaxLevelNum and rand)
//    {
//        k++;
//    }
//    LOG_INFO("k:{}",k);
//    return k;

    size_t k=1;
    while(rand()%2) {
        k++;
    }
    return std::min(k, this->max_level_);
}

template<typename Key,typename Value>
void SkipList<Key, Value>::Print() {
    std::shared_lock<std::shared_mutex> sl{mutex_};
//    auto p=head_->next_[0];
//    for(int i=0;i<elementCount_;++i) {
//        std::cout << "key_" << i << " = " << p->key_ << std::endl;
//        p = p->next_[0];
//    }
//    std::cout << "============= DEBUG =============" << std::endl;

    for(ssize_t i=max_level_-1;i>=0;--i) {
        printf("Level %ld: \n",i);
        auto p= this->head_->next_[i];
        while(p!= nullptr){
            std::cout<<"Key: "<<p->getKey()<<"\t"<<"Value: "<<p->getValue()<<" ";
            p=p->next_[i];
        }
        std::cout<<std::endl;
    }
}

template<typename Key,typename Value>
void SkipList<Key, Value>::SkipListIterator::MoveToFirst() {
    node_=list_->head_->next_[0];
}

template<typename Key,typename Value>
void SkipList<Key, Value>::SkipListIterator::MoveToLevel(size_t level) {
    node_=list_->head_->next_[level];
    LOG_INFO("{} | {}",node_.value()->getValue(),node_.value()->getKey());
}

template<typename Key,typename Value>
std::size_t SkipList<Key, Value>::SkipListIterator::Size() {
    return node_.value()->next_.size();
}


template<typename Key,typename Value>
void SkipList<Key, Value>::SkipListIterator::Next() {
    assert(Valid());
    node_=node_.value()->next_[0];
}

template<typename Key,typename Value>
bool SkipList<Key, Value>::SkipListIterator::Valid() {
    return node_.has_value();
}


template<typename Key,typename Value>
const Value &SkipList<Key, Value>::SkipListIterator::value() {
    assert(Valid());
    LOG_INFO("{}",node_.value()->getValue());
    //TODO
    //Why not node_value()->getValue();? It is tempory and panic
    return node_.value()->value_;
}

template<typename Key,typename Value>
const Key &SkipList<Key, Value>::SkipListIterator::key() {
    assert(Valid());
    return node_.value()->key_;
}

template<typename Key,typename Value>
SkipList<Key, Value>::SkipListIterator::SkipListIterator(const SkipList<Key, Value> *list)
                                                        :list_{list}
                                                        ,node_{std::nullopt}
{}



