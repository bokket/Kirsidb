//
// Created by bokket on 2024/2/2.
//
#pragma once

#include <memory>

#include "block_builder.h"

#define RESTART_INTERVAL 2

namespace bokket
{

class Block:public std::enable_shared_from_this<Block>
{

public:
    class Iterator{
    public:
        Iterator(std::shared_ptr<Block>&& container,size_t restarts_block_idx=0);

        Iterator()
                :restarts_block_idx_{0}
                ,entries_idx_{0}
                ,value_len_{0}
                ,unshared_key_len_{0}
                ,shared_key_len_{0}
                ,cur_entry_{nullptr}
                ,valid_{false}
        {}

        Iterator &operator++();

       // Iterator operator++(int);

     //   Iterator &operator=(const Iterator& rhs);

      //  Iterator(const Iterator& rhs);

       // Iterator &operator=(Iterator && rhs) noexcept;

      //  Iterator(Iterator &&rhs) noexcept;

        int operator<=>(Iterator &rhs) {
            assert(container_==rhs.container_);

            if(cur_entry_ > rhs.cur_entry_)
                return 1;
            else if(cur_entry_<rhs.cur_entry_)
                return -1;

            return 0;
        }

        operator bool() {
            if(!container_)
                return false;
            if(*this>=container_->end())
                return false;

            return true;
        }

        bool operator!=(Iterator &rhs) {
            return !(*this==rhs);
        }

        bool operator==(Iterator &rhs) {
            return container_==rhs.container_ and cur_entry_==rhs.cur_entry_;
        }

        auto GetContainer() {
            return container_;
        }

        void iter();

        //void FetchWithoutValue();

        bool Valid() {
            return valid_;
        }

        const std::string_view Key() const {
            return cur_key_;
        }
        const std::string_view Value() const {
            return cur_value_;
        }
    private:
      //  void setInValid();

        size_t restarts_block_idx_;
        size_t entries_idx_;

        std::shared_ptr<Block> container_;

        const char* cur_entry_;
        int32_t value_len_;
        int32_t unshared_key_len_;
        int32_t shared_key_len_;

        bool valid_;
        std::string cur_key_;
        std::string cur_value_;
    };

    friend class Iterator;


    Block()=default;

    Iterator begin() {
        return Iterator(shared_from_this(),0);
    }

    Iterator end() {
        return Iterator(shared_from_this(),restarts_.size());
    }

    DB Init(std::string_view data);

    std::vector<std::string_view> Get(std::string_view key);

    int Seek(std::string_view key);

//    void Gets(std::string_view);

    std::vector<std::string_view> ParseKey(int index, std::string_view& want);

    //bool ParseNextKey(int index);

    DB DecodeEntry(const char* restart_cord,std::string_view& restarts_key,std::string_view &restarts_value);

    int Compare(std::string_view a,std::string_view b) const{
        return a.compare(b);
    }

private:
    std::string_view data_;
    std::string_view data_buf_;

    std::size_t size_;

    int32_t restarts_offset_;

    std::vector<int32_t> restarts_;

};

}



