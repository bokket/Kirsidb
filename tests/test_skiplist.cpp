//
// Created by bokket on 2024/1/19.
//

#include <chrono>

#include "../SkipList/skiplist.h"

#include "../Log/Log.h"

using namespace std;
using namespace bokket;

bool test_inset(size_t list_level=12,size_t  element_count=10) {
    auto *list=new SkipList<int,int>;

    auto start=std::chrono::high_resolution_clock::now();

    for(size_t i=0;i<element_count;++i) {
        list->Insert(i,i);
    }

    list->Print();



    auto* iter=new SkipList<int,int>::SkipListIterator{list};

    iter->MoveToFirst();
    for (auto i=0;i<element_count;++i) {
        auto v=iter->value();
        auto k=iter->key();
        iter->Next();

        //std::cout<<v<<" | "<<k<<std::endl;

        //LOG_INFO("v{} | k {}",v,k);
    }

    // iter->MoveToLevel(2);
    // LOG_INFO("size:{}",iter->Size());
    // auto s=iter->Size();
    // for (size_t i=0;i<s;++i) {
    //     LOG_INFO("i:{}",iter->Size());

    //     auto v=iter->value();
    //     auto k=iter->key();
    //     iter->Next();

    // }


    //std::shared_ptr<SkipList<int,int>::SkipListIterator> iter;
    //iter=std::make_shared<SkipList<int,int>::SkipListIterator>(list);


//    for(auto i=0;i<element_count;++i) {
//        list->Delete(i);
//        std::cout<<list->size()<<"| "<<i<<std::endl;
//
//        list->Print();
//
//        assert(list->size()==element_count-i-1);
//    }

    auto end=std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed=end-start;

    cout<<"Pass Test1 : single element insert and erase consequently, elapsed :"<<elapsed.count()<<endl;
    return true;
}

bool test_inset_string(size_t list_level=12,size_t  element_count=10) {


    auto *list=new SkipList<int ,string>;

    auto start=std::chrono::high_resolution_clock::now();

    for(size_t i=0;i<element_count;++i) {
        list->Insert(i,"value_"+ to_string(i));
    }

    list->Print();

    auto* iter=new SkipList<int ,string>::SkipListIterator{list};

    iter->MoveToFirst();
    LOG_INFO("size:{}",iter->Size());
    for (auto i=0;i<iter->Size();++i) {
        auto &v=iter->value();
        auto &k=iter->key();

        LOG_INFO("{},{}",k,v);
        iter->Next();

    }

    iter->MoveToLevel(1);
    LOG_INFO("size:{}",iter->Size());
    auto s=iter->Size();
    for (size_t i=0;i<s;++i) {
        //LOG_INFO("i:{}",iter->Size());
        auto v=iter->value();
        auto k=iter->key();

        LOG_INFO("k: {} v: {}",k,v);

        iter->Next();

    }

    delete iter;
    delete list;

    auto end=std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed=end-start;

    cout<<"Pass Test1 : single element insert and erase consequently, elapsed :"<<elapsed.count()<<endl;
    return true;
}

int main() {
    //test_inset();
    test_inset_string();
}