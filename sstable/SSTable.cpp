#include "SSTable.h"

using namespace bokket;

SSTable::SSTable(ReadableFile* file)
                :file_(file)
                ,index_block_{nullptr}
                ,data_block_{nullptr}
{}

SSTable::~SSTable() {
    delete file_;
}

SSTable* SSTable::Open(ReadableFile* file, uint64_t file_size) {
    std::unique_ptr<SSTable> table = std::make_unique<SSTable>(file);

    table->index_block_=table->ReadIndexBlock(file_size);

    table->data_block_=table->ReadDataBlock(table->index_block_->begin().value());

    return table.release();
}

std::unique_ptr<Block> SSTable::ReadDataBlock(std::string_view index_value) {
    std::cout << index_value << std::endl;

    BlockHandle handle;

    stat_ = BlockHandle::DecodeFrom(index_value.data(), handle);

    handle.DebugString();

    if (!stat_) 
        return nullptr;

    std::string res;
    res.resize(handle.size_);

    LOG_INFO("{}, {}", handle.offset_, handle.size_);

    stat_ = file_->read(handle.offset_, handle.size_, &res);

    LOG_INFO("{}", res);

    auto data_block=std::make_unique<Block>(res);

    LOG_INFO("{}", data_block->begin().key());

    return data_block;
}

std::unique_ptr<Block> SSTable::ReadIndexBlock(uint64_t file_size) {
    std::string footer_content;
    footer_content.resize(32);

    auto s = file_->read(file_size - 32, 32, &footer_content);

    if (!s) LOG_ERROR("..............");

    Footer footer;
    s = Footer::DecodeFrom(footer_content, footer);

    footer.DebugString();

    if (!s) LOG_ERROR("..............");

    std::string index_value;
    index_value.resize(footer.index_block_.size_);

    s = file_->read(footer.index_block_.offset_, footer.index_block_.size_, &index_value);

    LOG_INFO("{}",index_value);

    auto index_block=std::make_unique<Block>(index_value);

    return index_block;
}

SSTable::Iter SSTable::begin() const {
    return {this, new BlockConstIter(index_block_->begin()),
            new BlockConstIter(data_block_->begin())};
    //return TwoLevelIterator(this,index_block_->begin(),&SSTable::ObtainBlockByIndex);
}

SSTable::Iter SSTable::end() const {
    return {};
    //return TwoLevelIterator(this,new BlockConstIter(index_block_->end()),new BlockConstIter(data_block_->end()));
}

SSTable::Iter SSTable::find(std::string_view key) const {

    auto index_iter = index_block_->find(key);

    if (index_iter == index_block_->end()) {
        return end();
    }

    auto block = ObtainBlockByIndex(index_iter);

    if (!block) {
        return end();
    }

    auto block_iter = block->find(key);
    if (block_iter == block->end()) {
        return end();
    }

    return {this, new BlockConstIter(index_iter), new BlockConstIter(block_iter)};
}

Status SSTable::find(std::string_view key,std::string& res) const {

    auto index_iter = index_block_->find(key);

    if (index_iter == index_block_->end()) {
        //return end();
    }

    auto block = ObtainBlockByIndex(index_iter);

    if (!block) {
        //return end();
    }

    auto block_iter = block->find(key);
    if (block_iter == block->end()) {
        //return end();
    }

    res=block_iter.value();

    delete block;

    return Status::OK();
}

Block* SSTable::ObtainBlockByIndex(const BlockConstIter& it) const {
    BlockHandle handle;
    auto index_value = it.value();
    stat_ = BlockHandle::DecodeFrom(index_value.data(), handle);

    handle.DebugString();

    if (!stat_) return nullptr;

    std::unique_ptr<Block> block = nullptr;

    LRUCachePolicy* cache = nullptr;

    if (cache) {
        // TODO
    }

    if (block != nullptr) {
    }

    std::string res;
    res.resize(handle.size_);

    LOG_INFO("{}, {}", handle.offset_, handle.size_);

    stat_ = file_->read(handle.offset_, handle.size_, &res);

    LOG_INFO("{}", res);

    if (stat_) {
        if (res.length() < handle.size_) stat_ = Status::IOError("ReadBlockFromFile error");
    }

    if (!stat_) return nullptr;


    block = std::make_unique<Block>(res);

    stat_ = Status::OK();

    return block.release();
}

TwoLevelIterator::TwoLevelIterator(const TwoLevelIterator& rhs) : table_(rhs.table_) {
    if (rhs.valid()) {
        LOG_INFO("TwoLevelIterator::TwoLevelIterator(const TwoLevelIterator &rhs)");

        data_iter_ = std::make_unique<BlockConstIter>(*rhs.data_iter_);
        index_iter_ = std::make_unique<BlockConstIter>(*rhs.index_iter_);

        table_ = rhs.table_;
    }
}

TwoLevelIterator& TwoLevelIterator::operator=(const bokket::TwoLevelIterator& rhs) {
    LOG_INFO("TwoLevelIterator::operator=");
    if (*this != rhs) {

        TwoLevelIterator tmp(rhs);
        std::swap(*this,tmp);
    }

    return *this;
}

bool TwoLevelIterator::equal(const bokket::TwoLevelIterator& other) const {
    if (!data_iter_ || !other.data_iter_) 
        return data_iter_.get() == other.data_iter_.get();

    return *data_iter_ == *other.data_iter_;
}

void TwoLevelIterator::increment() {
    assert(valid());

    ++(*data_iter_);

    if ((*data_iter_) == data_iter_->getContainer()->end()) {

        ++(*index_iter_);

        if ((*index_iter_) == index_iter_->getContainer()->end()) {
            data_iter_ = nullptr;
        } else {
            block_.reset(const_cast<SSTable*>(table_)->ObtainBlockByIndex(*index_iter_));

            data_iter_ = std::make_unique<BlockConstIter>(block_->begin());
        }
    }
}

inline bool TwoLevelIterator::valid() const {
    return *this != TwoLevelIterator();
}