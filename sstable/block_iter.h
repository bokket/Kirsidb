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

class SSTable;

class BlockConstIter;


// using BlockIterFacade=
//         IteratorFacadeNoValueType<BlockConstIter,ForwardIteratorTag, true>;

//template<class Ptr>
// class BlockConstIter:public BlockIterFacade

class BlockConstIter
{
public:
    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = BlockConstIter;
    using difference_type = std::ptrdiff_t;
    using pointer = BlockConstIter*;
    using reference = const value_type&; 
public:
    friend class Block;

//    using Iter_ptr=typename std::pointer_traits<Ptr>::template rebind<Block>;
    //class Block;
    explicit BlockConstIter(Block* container, const char* p, int32_t restart);


    BlockConstIter();

    //BlockConstIter(const char* begin,const char* end,const char* p,uint32_t restart);

    BlockConstIter(const BlockConstIter& rhs);

    BlockConstIter& operator=(const BlockConstIter& rhs);

    std::string_view key() const;

    std::string_view value() const;


    auto getContainer()const {
        return container_;
    }

    reference operator*() const noexcept;

    const BlockConstIter* operator->() const noexcept;
    BlockConstIter& operator++() noexcept;
    BlockConstIter operator++(int) noexcept;
    // BlockConstIter& operator--() noexcept;
    // BlockConstIter operator--(int) noexcept;
    bool operator==(const BlockConstIter& right) const noexcept;

    bool operator!=(const BlockConstIter &rhs) const noexcept {
        return !(*this == rhs); 
    }

    operator bool();

public:
    void increment();

    bool equal(const BlockConstIter& other) const;

    void init(const char *p);
private:
    //const char* buf_;
    std::string_view buf_;

    std::string last_key_;

    size_t restarts_block_idx_;
    size_t buf_len_;

    const Block* container_;
    //std::shared_ptr<Block> container_;
    uint32_t value_len_;
    uint32_t unshared_key_len_;
    uint32_t shared_key_len_;

    mutable std::string cur_key_;
    std::string_view cur_entry_;
    //mutable std::string cur_value_;
};


class Block
{
public:
    //Block()=default;
    //Status Init(std::string_view content);

    explicit Block(const std::string& content);
    ~Block()=default;

    friend class BlockConstIter;
    using Iter=BlockConstIter;

    Iter find(std::string_view key);

    [[nodiscard]] Iter begin();

    [[nodiscard]] Iter end() ;

    Iter lower_bound(std::string_view key) ;

    [[nodiscard]] int32_t restartPoint(int id) const;

    [[nodiscard]] std::string_view keyAtRestartPoint(int id) const;

    [[nodiscard]] static int Compare(std::string_view a,std::string_view b) {
        return a.compare(b);
    }

    [[nodiscard]] auto getData() const {
        return data_;
    }
private:
    std::string_view data_;
    //const char* data_;
    const char* data_end_{};
    std::size_t size_{};
    int32_t num_restart_{};
};

}