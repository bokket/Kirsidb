//
// Created by bokket on 2024/2/20.
//
#pragma once

#include <string>
#include <string_view>

#include "./coding.h"

#include "../Log/Log.h"
#include "../db/status.h"
#include "../iterator/iterator_trait.h"

namespace bokket {

class Block;

class BlockConstIter;

using BlockIterFacade =
    IteratorFacadeNoValueType<BlockConstIter, std::forward_iterator_tag, true>;

// template<class Ptr>
class BlockConstIter : public BlockIterFacade
{

public:
    friend class Block;
    friend class IteratorCoreAccess;

    BlockConstIter();

    BlockConstIter(const BlockConstIter &rhs);

    BlockConstIter &operator=(const BlockConstIter &rhs);

    std::string_view key() const;

    std::string_view value() const;
    
    auto getContainer() { return container_; }
private:
    //    using Iter_ptr=typename std::pointer_traits<Ptr>::template
    //    rebind<Block>;
    // class Block;
    BlockConstIter(const Block *container, const char *p, uint32_t restart);

    void increment();

    bool equal(const BlockConstIter &other) const;

    void init(const char *p);

private:
    // const char* buf_;
    std::string_view buf_;

    // const char* last_key_;
    std::string last_key_;

    size_t restarts_block_idx_;
    size_t buf_len_;

    const Block *container_;
    // std::shared_ptr<Block> container_;
    uint32_t value_len_;
    uint32_t unshared_key_len_;
    uint32_t shared_key_len_;

    mutable std::string cur_key_;
    std::string_view cur_entry_;
};

struct BlockContent
{
    explicit BlockContent(const std::string &data) : data(data) {}
    const std::string &data;
};

class Block
{
public:
    explicit Block(const std::string &content);
    explicit Block(std::string_view content);
    explicit Block(const BlockContent &content);

    Block() = default;

    friend class BlockConstIter;
    using Iter = BlockConstIter;

    [[nodiscard]] Iter find(std::string_view key) const;

    [[nodiscard]] Iter find_if(std::string_view key) const;

    [[nodiscard]] Iter lower_bound(std::string_view key) const;

    [[nodiscard]] Iter begin() const;

    [[nodiscard]] Iter end() const;
private:
    Status Init(std::string_view content);

    [[nodiscard]] uint32_t restartPoint(int id) const;

    [[nodiscard]] std::string_view keyAtRestartPoint(int id) const;

    [[nodiscard]] static int Compare(std::string_view a, std::string_view b)
    {
        //return a.compare(b);
        LOG_INFO("{},{}",a,b);
        const size_t min_len = (a.size() < b.size()) ? a.size() : b.size();
        int r = memcmp(a.data(), b.data(), min_len);
        
        if (r == 0) {
            if (a.size() < b.size())
                r = -1;
            else if (a.size() > b.size())
                r = +1;
        }
        return r;
    
    }

private:
    //std::string_view data_;
    std::string data_;
    const char *data_end_;
    std::size_t size_;
    uint32_t num_restart_;
};

} // namespace bokket