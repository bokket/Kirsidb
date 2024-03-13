//
// Created by bokket on 2024/2/20.
//
#pragma once


#include <memory>

#include "block_builder.h"
#include "../iterator/iterator_trait.h"

namespace bokket
{

class Block;

class BlockConstIter;


using BlockIterFacade=
        IteratorFacadeNoValueType<BlockConstIter,ForwardIteratorTag, true>;

//template<class Ptr>
class BlockConstIter:public BlockIterFacade
{

public:
    friend class Block;

//    using Iter_ptr=typename std::pointer_traits<Ptr>::template rebind<Block>;
    //class Block;
    BlockConstIter(const Block* container, const char* p, int32_t restart);

    std::string_view key() const;

    std::string_view value() const;

    auto getContainer() {
        return container_;
    }

public:
    void increment();

    bool equal(const BlockConstIter& other) const;

    void init(const char* p);
private:
    const char* buf_;
    const char* last_key_;

    size_t restarts_block_idx_;
    size_t buf_len_;

    const Block* container_;
    //std::shared_ptr<Block> container_;
    int32_t value_len_;
    int32_t unshared_key_len_;
    int32_t shared_key_len_;

    mutable std::string cur_key_;
    //mutable std::string cur_value_;
};


class Block:public std::enable_shared_from_this<Block>
{
public:
    Block(std::string_view content);

    Block()=default;


    friend class BlockConstIter;
    using Iter=BlockConstIter;

    Iter find(std::string_view key);

    Iter begin() const;

    Iter end() const;

    Iter lower_bound(std::string_view key) const;

    int32_t restartPoint(int id) const;

    std::string_view keyAtRestartPoint(int id) const;

    int Compare(std::string_view a,std::string_view b) const{
        return a.compare(b);
    }
private:
    std::string_view data_;
    const char* data_end_;
    std::size_t size_;
    int32_t num_restart_;
};

}