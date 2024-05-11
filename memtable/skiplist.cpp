#include "skiplist.h"

using namespace bokket;



template<typename Key,class Comparator>
typename SkipList<Key, Comparator>::Iter SkipList<Key,Comparator>::Begin() const
{
    //return Iter(this,head_->next_[0]);
    LOG_INFO("begin");
    return Iter{this};
}


template<typename Key,class Comparator>
typename SkipList<Key, Comparator>::Iter SkipList<Key,Comparator>::End() const
{
    return Iter{nullptr};
}


template<typename Key,class Comparator>
SkipList<Key,Comparator>::SkipList(Comparator cmp)
                                        :cmp_{cmp}
                                        ,head_{nullptr}
                                        ,max_level_{1}
{
    (*const_cast<Node**>(&head_))=From(0, kMaxLevelNum);
}

template<typename Key,class Comparator>
SkipList<Key,Comparator>::~SkipList()
{
    if(head_!=nullptr)
    {
        LOG_INFO("?");
        delete head_;
    }
}


template<typename Key,class Comparator>
typename SkipList<Key,Comparator>::Node* SkipList<Key,Comparator>::From(const Key& k,int level) {
    LOG_INFO("skiplist new node k:{}",k);
    return new Node(k,level);
}

// Insert given key and value in skip list
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                       100
                 |
                 |                      insert  +----+
level 3         1+-------->10+----------------> | 50 |          70       100
                                                |    |
                                                |    |
level 2         1          10          30       | 50 |          70       100
                                                |    |
                                                |    |
level 1         1   4      10          30       | 50 |          70       100
                                                |    |
                                                |    |
level 0         1   4   9  10          30   40  | 50 |  60      70       100
                                                +----+
*/
template<typename Key,class Comparator>
void SkipList<Key,Comparator>::Insert(const Key& key)
{
    std::unique_lock<std::shared_mutex> ul{mutex_};

    std::vector<Node*> prev(kMaxLevelNum, nullptr);

    Node* cur=head_;

    for(int i=GetMaxHeight()-1;i>=0;--i)
    {
        while(cur->next_[i]!=nullptr and cmp_(cur->next_[i]->getKey(),key)<0)
        {
            cur=cur->next_[i];
        }
        prev[i]=cur;
    }

    cur=cur->next_[0];
    if(cur!=nullptr and cmp_(cur->getKey(),key)==0)
        return;
        //return Iter{this,cur};

    assert(!Contains(key));
    

    int level=getRandomLevel();
    if(level>GetMaxHeight())
    {
        for(int i=GetMaxHeight();i<level;++i)
            prev[i]=head_;

        //max_level_.store(level,std::memory_order_relaxed);
        max_level_=level;
    }

    LOG_INFO("{}",level);
    auto newNode=From(key, level);

    for(int i=level-1;i>=0;--i)
    {
        if(prev[i]==nullptr)
        {
            newNode->next_[i]=nullptr;
            head_->next_[i]=newNode;
        }
        newNode->next_[i]=prev[i]->next_[i];
        prev[i]->next_[i]=newNode;
    }

    //return Iter{this,newNode};
}


template<typename Key,class Comparator>
std::vector<typename SkipList<Key,Comparator>::Node*> SkipList<Key,Comparator>::FindPrevNode(const Key &key) {
    std::vector<Node*> prev(kMaxLevelNum,nullptr);


    Node* cur=head_;

    for(int i=GetMaxHeight()-1;i>=0;--i)
    {

        auto next=cur->next_[i];

        while(next!=nullptr and cmp_(next->getKey(),key)<0)
        {
            cur=next;

            next=cur->next_[i];
        }

        prev[i]=cur;
    }

    return prev;
}


template<typename Key,class Comparator>
typename SkipList<Key, Comparator>::Iter SkipList<Key,Comparator>::Find(const Key& target) const
{
    //std::shared_lock<std::shared_mutex> sl{mutex_};

    auto cur=head_;

    for(ssize_t i=GetMaxHeight()-1;i>=0;--i)
    {
        auto next=cur->next_[i];
        while(next!=nullptr and cmp_(next->getKey(),target)<0)
        {
            cur=next;
            next=cur->next_[i];
        }
        if(next!=nullptr and cmp_(next->getKey(),target)==0)
        {    
            LOG_INFO("Get key:{}",cur->next_[i]->getKey());
            return Iter{this,next};
        }
    }

    return End();

}


template<typename Key,class Comparator>
void SkipList<Key,Comparator>::Print() {
    std::shared_lock<std::shared_mutex> sl{mutex_};

    Node* ptr = nullptr;
    static const int PRINT_WIDTH = 3;

     for (int i = GetMaxHeight()-1; i >= 0; --i) {
        auto ptr = head_->next_[0];
        //printf("level: %d: ", i);
        std::cout<<"level: "<<i<<"| ";

        if (ptr == nullptr)  // 如果链表为空，则打印 nullptr
            std::cout << std::right << std::setw(PRINT_WIDTH * 2) << "| nullptr";

        while (ptr) {
            if (ptr->getLevel() > i) {
                std::cout << std::right << std::setw(PRINT_WIDTH) << ptr->key_;
                if (ptr->next_[i]) {
                    std::cout << std::left << std::setw(PRINT_WIDTH) << " (->" + std::to_string(ptr->next_[i]->key_) + ")";
                } else {
                    std::cout << std::right << std::setw(PRINT_WIDTH * 2) << "| nullptr";
                }
                
            } else {
                std::cout << std::right << std::setw(PRINT_WIDTH * 2) << " ";
            }
            ptr = ptr->next_[0];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


template<typename Key,class Comparator>
bool SkipList<Key,Comparator>::Contains(const Key& key) const
{
    return Find(key)!=End();
}

template<typename Key,class Comparator>
int SkipList<Key,Comparator>::GetMaxHeight() const
{
    return max_level_.load(std::memory_order_relaxed);
}


template<typename Key,class Comparator>
int SkipList<Key,Comparator>::getRandomLevel()
{
    static const unsigned int kBranching = 4;  // 控制高度的参数，可以根据需要调整
    int height = 1;
    
    std::random_device rd;  // 用于生成随机数种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
    std::uniform_real_distribution<double> dis(0.0, 1.0); // 均匀分布的随机数生成器

    while (height < kMaxLevelNum && dis(gen) < 1.0 / kBranching) {
        height++;
    }

    return height;
}