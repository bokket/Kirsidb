//
// Created by bokket on 24-3-26.
//

#pragma once

#include_next "../iterator/iterator_trait.h"

namespace bokket
{

class Block;
class BlockConstIter;
class SSTable;
class TwoLevelIterator;

using TwoLevelIterFacade=
        IteratorFacadeNoValueType<TwoLevelIterator,ForwardIteratorTag, true>;

class TwoLevelIterator:public TwoLevelIterFacade
{
    friend class SSTable;
public:
    TwoLevelIterator(BlockConstIter* data_iter,BlockConstIter* index_iter,const SSTable* table);
    TwoLevelIterator()=default;
public:
    TwoLevelIterator(const TwoLevelIterator& rhs);

    TwoLevelIterator& operator=(const TwoLevelIterator& rhs);

    ~TwoLevelIterator()=default;

    std::string_view Key() const;

    std::string_view Value() const;
public:
    void increment();

    bool equal(const TwoLevelIterator& other) const;

    bool valid() const;
private:
    std::unique_ptr<BlockConstIter> data_iter_;
    std::unique_ptr<BlockConstIter> index_iter_;

    std::shared_ptr<const Block> block_;

    const SSTable* table_;

};

}
