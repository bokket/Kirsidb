#include "SSTable.h"
#include "block_iter.h"
#include "format.h"
#include <cstddef>
#include <memory>
#include <utility>

using namespace bokket;


SSTable::SSTable(ReadableFile* file)
                            :file_(file)
                            ,index_block_(nullptr)
                            ,data_block_(nullptr)
{}

SSTable::~SSTable()
{
    if(file_)
    {
        LOG_INFO("{?}");
        delete file_;

    }
}


Status SSTable::Open(ReadableFile* file,SSTable** table,uint64_t file_size)
{
    auto* sst=new SSTable(file);

    auto s=sst->ReadFooterBlock(file_size);

    s=sst->ReadIndexBlock();

    //s=sst->ObtainBlockByIndex(file);

    *table=sst;

    return Status::OK();
}

Status SSTable::ReadIndexBlock()
{
    std::cout<<index_value_<<std::endl;
    
    index_block_=std::make_unique<Block>(index_value_);
    //auto s=index_block_->Init(index_value_);

    BlockHandle handle;

    auto index_value=index_block_->begin().value();

    stat_=BlockHandle::DecodeFrom(index_value.data(),handle);

    handle.DebugString();

    if(!stat_)
        return Status::Corruption("SSTable::ObtainBlockByIndex");
    
    //std::unique_ptr<Block> data_block_iter;
    // TODO
    //LRUCachePolicy* cache= nullptr;

    //     if(cache) {
    //         // TODO

    //     }


    std::string res;
    res.resize(handle.size_);

    LOG_INFO("{}, {}",handle.offset_,handle.size_);

    stat_=file_->read(handle.offset_,handle.size_,&res);

    LOG_INFO("{}",res);


    //data_block_=std::make_unique<Block>();
    block_value_=res;

    data_block_=std::make_unique<Block>(block_value_);
    //data_block_->Init(block_value_);

    LOG_INFO("{}",data_block_->begin().key());

    return stat_;
}


Status SSTable::ReadFooterBlock(uint64_t file_size) 
{
    std::string footer_content;
    footer_content.resize(32);

    auto s=file_->read(file_size-32,32,&footer_content);

    if(!s)
        LOG_ERROR("..............");

    Footer footer;
    s=Footer::DecodeFrom(footer_content,footer);

    footer.DebugString();

    if(!s)
        LOG_ERROR("..............");

    std::string index_value;
    index_value.resize(footer.index_block_.size_);

    s=file_->read(footer.index_block_.offset_,footer.index_block_.size_,&index_value);

    index_value_=index_value;

    return s;
}

// BlockConstIter SSTable::ObtainBlockByIndex(void* table,std::string_view index_value)
// {

//     auto* sst=reinterpret_cast<SSTable*>(table);

//     Block* block=nullptr;

//     auto value=index_value;

//     BlockHandle handle;


//     auto stat=BlockHandle::DecodeFrom(index_value.data(),handle);

//     handle.DebugString();

//     // if(!stat)
//     //     return ;
    
//     //std::unique_ptr<Block> data_block_iter;
//     // TODO
//     //LRUCachePolicy* cache= nullptr;

//     //     if(cache) {
//     //         // TODO

//     //     }


//     std::string res;
//     res.resize(handle.size_);

//     LOG_INFO("{}, {}",handle.offset_,handle.size_);

//     stat=sst->file_->read(handle.offset_,handle.size_,&res);

//     LOG_INFO("{}",res);

//     block=new Block();

//     block->Init(res);

//     LOG_INFO("{},{}",block->begin().key(),block->begin().value());

//     return block->begin();
// }


SSTable::Iter SSTable::begin()
{


    //LOG_INFO("{},{}",data_block_->begin().key(),data_block_->begin().value());
    return {this,new BlockConstIter(index_block_->begin()),new BlockConstIter(data_block_->begin())};
    //return TwoLevelIterator(this,index_block_->begin(),&SSTable::ObtainBlockByIndex);
}

SSTable::Iter SSTable::end()
{
    return {};
    //return TwoLevelIterator(this,new BlockConstIter(index_block_->end()),new BlockConstIter(data_block_->end()));
}


Status SSTable::ObtainBlockByIndex(BlockHandle& handle,Block&& data_block) const
{
    std::string res;
    res.resize(handle.size_);

    LOG_INFO("{}, {}",handle.offset_,handle.size_);

    stat_=file_->read(handle.offset_,handle.size_,&res);

    LOG_INFO("{}",res);

    //data_block.Init(res);

    return stat_;
}

Block* SSTable::ObtainBlockByIndex(const BlockConstIter& it)
{
    BlockHandle handle;
    auto index_value=it.value();
    stat_=BlockHandle::DecodeFrom(index_value.data(),handle);

    handle.DebugString();

    if(!stat_)
        return nullptr;

    Block* block=nullptr;
    LRUCachePolicy* cache= nullptr;

    if(cache) {
        // TODO

    }

    if(block== nullptr) {
        std::string res;
        res.resize(handle.size_);

        LOG_INFO("{}, {}",handle.offset_,handle.size_);

        stat_=file_->read(handle.offset_,handle.size_,&res);

        LOG_INFO("{}",res);

        if(stat_) {
            if(res.length()<handle.size_)
                stat_=Status::IOError("ReadBlockFromFile error");
        }

        if(!stat_)
            return nullptr;

        block_value_=res;

        block=new Block(block_value_);

        stat_=Status::OK();
    }
    return block;
}


TwoLevelIterator::TwoLevelIterator(const TwoLevelIterator &rhs)
                                  :table_(rhs.table_)
{
    if(rhs.valid()) {
        LOG_INFO("TwoLevelIterator::TwoLevelIterator(const TwoLevelIterator &rhs)");

        data_iter_.reset(new BlockConstIter(*rhs.data_iter_));
        index_iter_.reset(new BlockConstIter(*rhs.index_iter_));

        table_ = rhs.table_;

        //data_iter_=rhs.data_iter_;
        //index_iter_=rhs.index_iter_;
        //table_=rhs.table_;
    }
}

TwoLevelIterator &TwoLevelIterator::operator=(const bokket::TwoLevelIterator &rhs) {
    LOG_INFO("TwoLevelIterator::operator=");
    if(*this!=rhs)
    {

        // if(rhs.data_iter_)

        table_=rhs.table_;
        
        data_iter_ = std::make_unique<BlockConstIter>(*rhs.data_iter_);
        index_iter_ = std::make_unique<BlockConstIter>(*rhs.index_iter_);
    }

    return *this;
    // TwoLevelIterator tmp(rhs);
    // std::swap(*this,tmp);
    // return *this;
}


bool TwoLevelIterator::equal(const bokket::TwoLevelIterator &other) const {
    if(!data_iter_ || !other.data_iter_)
        return data_iter_.get()==other.data_iter_.get();

    return *data_iter_==*other.data_iter_;
}


void TwoLevelIterator::increment()
{
    assert(valid());

    ++(*data_iter_);

    if((*data_iter_)==data_iter_->getContainer()->end()) {
        LOG_INFO("[  ]");

        ++(*index_iter_);
        
        if((*index_iter_)==index_iter_->getContainer()->end()) {
            LOG_INFO("?");

            data_iter_=nullptr;
            //index_iter_=nullptr;
            //TwoLevelIterator tmp;
            //*this=tmp;
            //std::swap(*this,tmp);

        } else {
            LOG_INFO("?");

            auto block=const_cast<SSTable*>(table_)->ObtainBlockByIndex(*index_iter_);
            TwoLevelIterator tmp(const_cast<SSTable*>(table_),
                                new BlockConstIter((*index_iter_)),new BlockConstIter(block->begin()));
            std::swap(*this,tmp);
        }
    }
}

inline bool TwoLevelIterator::valid() const {
    return *this!=TwoLevelIterator();
}

// void TwoLevelIterator::ReadDataIter()
// {
//     BlockHandle handle;
//     BlockHandle::DecodeFrom(index_iter_.value().data(),handle);

//     handle.DebugString();

//     auto index_value=index_iter_.value();

//     //auto data_iter=(*block_function_)(table_,index_value);

//     // Block data_block;

//    // data_iter_=data_iter;

//     //LOG_INFO("{}",data_iter_.key());
//     // table_->ObtainBlockByIndex(handle,std::move(data_block));

//     // data_iter_=data_block.begin();
// }