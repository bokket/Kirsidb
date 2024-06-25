//
// Created by bokket on 24-4-2.
//

#include "SSTable_builder.h"
#include <cstddef>

using namespace bokket;

SSTableBuilder::SSTableBuilder(const Options *options, WritableFile *file)
                             :options_{options}
                             ,data_block_{}
                             ,index_block_{}
                             ,file_(file)
                             ,pending_index_entry_{false}
                             ,num_entries_{0}
                             ,block_offset_{0}
{}

// *start:    helloWorld
// limit:     helloZookeeper
// 由于 *start < limit, 所以调用 FindShortest(start, limit)之后，start变成：
// UVWXYZ
// hello + (W+1)X = helloX
// helloX (保留前缀，第一个不相同的字符+1)
void SSTableBuilder::FindShortest(std::string &start, std::string_view limit) const {
    auto sep=std::min(start.size(), limit.size());
    size_t diff=0;
    for(size_t i=0;i<sep;++i) {
        if(start[i]!=limit[i])
            break;

        ++diff;
    }
    if(diff<sep) {
        if(static_cast<uint8_t>(start[diff])<static_cast<uint8_t>(0xff) && start[diff]+1 <limit[diff] ) {
            start[diff]++;

            start.resize(diff+1);

            assert(start.compare(limit)<0);
        }
    }

    //    auto sep = start.length();
//    for(size_t i=0;i<start.length();++i) {
//        assert(i<limit.length());
//
//        if(start.at(i) == limit[i])
//            continue;
//
//        assert(start[i]<limit[i]);
//
//        // 127
//        if(start[i]<std::numeric_limits<char>::max())
//            start[i]=static_cast<char>(start[i]+1);
//
//        sep=i;
//        break;
//    }
//
//    if(sep== start.length()) {
//        assert(limit.length()>sep);
//        assert(limit[sep]!=0);
//
//        start.push_back(static_cast<char>(limit[sep]-1));
//    }


}


Status SSTableBuilder::Add(std::string_view key, std::string_view value) {
    //assert(!file_->close());

    if(pending_index_entry_) {
        assert(data_block_.empty());

        FindShortest(last_key_,key);

        LOG_INFO("last_key:{} | key:{}",last_key_,key);

        auto index_value=pending_handle_.EncodeToString();

        pending_handle_.DebugString();

        LOG_INFO("index_value:{}",index_value);

        index_block_.add(last_key_,index_value);
        pending_index_entry_= false;
    }

    ++num_entries_;
    data_block_.add(key,value);
    last_key_.assign(key);

    LOG_INFO("{}",options_->block_size);

    if(data_block_.size()>=options_->block_size) {
        LOG_INFO("{}, {}",data_block_.size(),options_->block_size);
        return flush();
    }

    return Status::OK();
}

Status SSTableBuilder::flush() {
    //assert(!file_->close());

    //assert(!pending_index_entry_);


    //
    if(data_block_.empty())
        return Status::OK();

    auto s= writeBlock(&data_block_,pending_handle_);
    if(!s)
        return s;

    pending_index_entry_= true;

    s=file_->flush();

    if(!s)
        return s;

    // data_block_.clear();

    return Status::OK();
}

Status SSTableBuilder::writeBlock(BlockBuilder *block, BlockHandle &handle) {
    auto block_buf=block->final();

    LOG_INFO("{}",block_buf);

    writeBytesBlock(block_buf,handle);

    block->clear();

    // data_block_.clear();

//    auto s=file_->append(block_buf);
//    if(!s)
//        return s;

    //crc32?

//    uint64_t block_size=block_buf.length();
//
//    pending_handle_.DebugString();
//
//    pending_handle_.size_=block_size;
//    pending_handle_.offset_+=block_size;
//
//    pending_handle_.DebugString();

    return Status::OK();
}

void SSTableBuilder::writeBytesBlock(std::string_view data, BlockHandle &handle) {
    //bool compress=false;

    handle.offset_=block_offset_;
    handle.size_=data.size();

    LOG_INFO("{},{}",handle.offset_,handle.size_);

    LOG_INFO("{}",data);
    file_->append(data);

    block_offset_+=data.size();

    LOG_INFO("{}",block_offset_);
}

Status SSTableBuilder::Finish() {
    auto s=flush();

    //assert(!file_->close());

    if(!s)
        return s;

    BlockHandle index_handle;

    if(pending_index_entry_) {

        auto index_value=pending_handle_.EncodeToString();

        pending_handle_.DebugString();
        LOG_INFO("index_value:{}",index_value);

        index_block_.add(last_key_,index_value);
        pending_index_entry_= false;
    }

   //index_block_.add(last_key_,pending_handle_.EncodeToString());


    s= writeBlock(&index_block_,index_handle);
    //s=writeBlock(&index_block_);
    if(!s)
        return s;

    Footer footer;
    footer.index_block_=index_handle;
    //footer.index_block_=pending_handle_;

    s=file_->append(footer.EncodeToString());

    footer.DebugString();

    block_offset_+=footer.EncodeToString().size();

    file_->close();

    return Status::OK();
}

Status SSTableBuilder::writeBlock(BlockBuilder *block)
{
    auto block_buf=block->final();

    LOG_INFO("{}",block_buf);

    auto s=file_->append(block_buf);
    if(!s)
        return s;

    //crc32?

    uint64_t block_size=block_buf.length();

    pending_handle_.DebugString();

    pending_handle_.size_=block_size;
    pending_handle_.offset_+=block_size;

    pending_handle_.DebugString();

    return Status::OK();
}

size_t SSTableBuilder::NumEntries() const
{
    return num_entries_;
}

uint64_t SSTableBuilder::fileSize() const
{
    return block_offset_;
}