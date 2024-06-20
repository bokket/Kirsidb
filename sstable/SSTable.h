#pragma once

#include <memory>
#include <string_view>

#include "Options.h"
#include "footer.h"
#include "format.h"
#include "block_iter.h"

#include "../cache/cache.h"
#include "../file/file_writer.h"

namespace bokket {

class SSTable;
class TwoLevelIterator;

// typedef BlockConstIter (*BlockFunction)(void* arg,
//                                 std::string_view index_value);
//using BlockFunction=std::function<Block*(SSTable*,std::string_view )>;

using TwoLevelIterFacade = IteratorFacadeNoValueType<TwoLevelIterator, ForwardIteratorTag, true>;

class TwoLevelIterator : public TwoLevelIterFacade {
public:
    friend class SSTable;

public:
    // TwoLevelIterator(SSTable* table,BlockConstIter&& index_iter,BlockFunction block_function)
    //                         :table_(table)
    //                         ,index_iter_{index_iter}
    //                         ,data_iter_{}
    //                         ,block_function_(std::move(block_function))
    TwoLevelIterator(const SSTable* table, BlockConstIter* index_iter, BlockConstIter* data_iter)
            //explicit TwoLevelIterator(SSTable* table,BlockConstIter&& index_iter,BlockConstIter&& data_iter)
            : table_(table), index_iter_ {index_iter}, data_iter_ {data_iter} {
        LOG_INFO("?");
    }

    TwoLevelIterator() { LOG_INFO("?"); }

    ~TwoLevelIterator() { LOG_INFO("?"); }

    TwoLevelIterator(const TwoLevelIterator& rhs);

    TwoLevelIterator& operator=(const TwoLevelIterator& rhs);

    [[nodiscard]] std::string_view Key() const {
        LOG_INFO("{?}");
        return data_iter_->key();
    }

    [[nodiscard]] std::string_view Value() const {
        LOG_INFO("{?}");
        return data_iter_->value();
    }

public:
    void increment();

    [[nodiscard]] bool equal(const TwoLevelIterator& other) const;

    [[nodiscard]] inline bool valid() const;

private:
    // BlockConstIter index_iter_;
    // BlockConstIter data_iter_;
    std::unique_ptr<BlockConstIter> index_iter_;
    std::unique_ptr<BlockConstIter> data_iter_;
    const SSTable* table_ {};

    std::unique_ptr<Block> block_;

    //BlockFunction block_function_;
};

class SSTable {
public:
    explicit SSTable(ReadableFile* file);

    ~SSTable();

    static SSTable* Open(ReadableFile* file, uint64_t file_size);

    //static BlockConstIter ObtainBlockByIndex(void* table,std::string_view index_value);

    // Status ObtainBlockByIndex(const BlockConstIter& it, std::unique_ptr<Block>& data_block) const;

    Block* ObtainBlockByIndex(const BlockConstIter& it) const;

    std::unique_ptr<Block>  ReadIndexBlock(uint64_t file_size);

    std::unique_ptr<Block>  ReadDataBlock(std::string_view index_value);

    friend class TwoLevelIterator;
    using Iter = TwoLevelIterator;
    Iter begin() const;

    Iter end() const;

    Iter find(std::string_view key) const;

private:
    mutable Status stat_;
    ReadableFile* file_;
    //const Options* options_;
    std::string index_value_;
    mutable std::string block_value_;

    std::unique_ptr<Block> index_block_;
    std::unique_ptr<Block> data_block_;
};
} // namespace bokket
