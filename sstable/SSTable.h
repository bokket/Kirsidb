#pragma once

#include <memory>
#include <string_view>
#include <utility>
//#include <format>

#include "./footer.h"
#include "./Options.h"
#include "./block_iter.h"

#include "../file/file_writer.h"
#include "../cache/cache.h"
#include "format.h"

namespace bokket
{

class SSTable;
class TwoLevelIterator;

// typedef BlockConstIter (*BlockFunction)(void* arg,
//                                 std::string_view index_value);
//using BlockFunction=std::function<Block*(SSTable*,std::string_view )>;

using TwoLevelIterFacade=
        IteratorFacadeNoValueType<TwoLevelIterator,ForwardIteratorTag, true>;

class TwoLevelIterator
{

public:
    friend class SSTable;
public:
        // TwoLevelIterator(SSTable* table,BlockConstIter&& index_iter,BlockFunction block_function)
        //                         :table_(table)
        //                         ,index_iter_{index_iter}
        //                         ,data_iter_{}
        //                         ,block_function_(std::move(block_function))
        TwoLevelIterator(SSTable* table,BlockConstIter&& index_iter,BlockConstIter&& data_iter)
                        :table_(table)
                        ,index_iter_{index_iter}
                        ,data_iter_{data_iter}
        {
            LOG_INFO("?");

            //ReadDataIter();
        }

        TwoLevelIterator()=default;

        // TwoLevelIterator(const TwoLevelIterator &rhs) 
        //                  :table_(rhs.table_) 
        // {
        //     if (rhs.index_iter_) 
        //         index_iter_ = *rhs.index_iter_;
        //     if (rhs.data_iter_) 
        //         data_iter_ = *rhs.data_iter_;
        // }

        // TwoLevelIterator &operator=(TwoLevelIterator &&rhs) noexcept 
        // {
        //     if (this != &rhs) {
        //         table_ = std::move(rhs.table_);
        //         index_block_ = std::move(rhs.index_iter_);
        //         data_iter_ = rhs.data_iter_;
        //     }
        //     return *this;
        // }

        TwoLevelIterator(TwoLevelIterator &&rhs) noexcept
                        :table_(std::move(rhs.table_))
                        ,index_iter_(rhs.index_iter_)
                        ,data_iter_(rhs.data_iter_) 
        {}

        std::string_view Key() const {
            LOG_INFO("{?}");
            return data_iter_.key();
        }

        std::string_view Value() const {
            LOG_INFO("{?}");
            return data_iter_.value();
        }

public:
    void increment();

   // bool equal(const TwoLevelIterator& other) const;

    inline bool valid() const;
private:
    BlockConstIter index_iter_;
    BlockConstIter data_iter_;
    SSTable* table_{};

    //BlockFunction block_function_;
};




class SSTable
{
public:
    explicit SSTable(ReadableFile* file);

    ~SSTable();

    static Status Open(ReadableFile* file,SSTable** table,uint64_t file_size);

    //static BlockConstIter ObtainBlockByIndex(void* table,std::string_view index_value);

    Status ObtainBlockByIndex(BlockHandle& handle,Block&& data_block) const;


    Status ReadFooterBlock(uint64_t file_size);

    Status ReadIndexBlock();

    friend class TwoLevelIterator;
    using Iter=TwoLevelIterator;
    Iter begin();

    Iter end();

private:
    mutable Status stat_;
    ReadableFile* file_;
    //const Options* options_;
    std::string index_value_;
    std::string block_value_;

    std::unique_ptr<Block> index_block_;
    std::unique_ptr<Block> data_block_;
};
}
