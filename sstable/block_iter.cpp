//
// Created by bokket on 2024/2/20.
//
#include <memory>

#include "block_iter.h"

using namespace bokket;

Block::Block(std::string_view content)
            :data_(content)
            ,size_(content.size())
{
    size_t restarts_len_offset=size_- sizeof(int32_t);
    num_restart_=DecodeFixed32(data_.data()+restarts_len_offset);

    assert(size_>=4*(num_restart_+1));

    data_end_=data_.data()+size_-4*(num_restart_+1);

    LOG_INFO("block_iter size_:{},num_restart_:{},data_end_:{}",size_,num_restart_,data_end_);
}

Block::Iter Block::find(std::string_view key) {
    auto it= lower_bound(key);
    return Compare(it.key(),key)!=0 ? end() : it;
}

Block::Iter Block::begin() const {
    return Iter(this,data_.data(),0);
}

Block::Iter Block::end() const {
    return Iter(this,data_end_,0);
}

int32_t Block::restartPoint(int id) const {
    assert(id<=num_restart_);
    int32_t restart_len=size_-4*(num_restart_-id+1);
    int32_t r= DecodeFixed32(data_.data()+restart_len);

    assert(r<=data_end_-data_.data());

    return r;
}

std::string_view Block::keyAtRestartPoint(int id) const {
    int32_t shared,unshared,value_len;

    int32_t pos= restartPoint(id);

    std::string_view buf{data_.data()+pos,static_cast<std::string_view::size_type>(data_end_-data_.data()-pos) };
    shared=DecodeFixed32(buf.data());
    assert(shared==0);

    unshared= DecodeFixed32(buf.data()+sizeof(int32_t));
    value_len= DecodeFixed32(buf.data()+sizeof(int32_t)*2);

    return std::string_view {buf.data()+sizeof(int32_t)*3,static_cast<std::string_view::size_type>(unshared)};
}

Block::Iter Block::lower_bound(std::string_view key) const {
    int left=0;
    int right=num_restart_-1;

    while(left<right) {
        int mid=(left+right)>>1;

        if(Compare(keyAtRestartPoint(mid),key)>=0) {
            right=mid;
        } else {
            left=mid;
        }
    }

    if(Compare(keyAtRestartPoint(left),key)>0) {
        assert(left==0 && right==left+1);
        return end();
    }

    int32_t pos= restartPoint(left);

    auto it=Iter(this,data_.data()+pos,pos);

    for(;it!=end();it++) {
        if(Compare(it.key(),key)>=0)
            break;
    }
    return it;
}

BlockConstIter::BlockConstIter(const Block *container, const char *p, int32_t restart)
        :last_key_{nullptr}
        ,container_{container}
        ,restarts_block_idx_(restart)
{
    init(p);
}

void BlockConstIter::init(const char *p) {
    auto len=container_->data_end_-p;
    assert(len>=0);
    if(len>0) {
        shared_key_len_=DecodeFixed32(p);
        unshared_key_len_= DecodeFixed32(p+sizeof(int32_t));
        value_len_= DecodeFixed32(p+sizeof(int32_t)*2);

        buf_=std::string_view {p+sizeof(int32_t)*3,static_cast<std::string_view::size_type>(len)}.data();
        buf_len_=len;

        if(unshared_key_len_==0) {
            restarts_block_idx_=static_cast<int32_t>(p-container_->data_.data());
        }
    } else {
        buf_=p;
        buf_len_=0;
    }
}


void BlockConstIter::increment() {
    last_key_=buf_;
    init(buf_+unshared_key_len_+value_len_);
    cur_key_.clear();
}

bool BlockConstIter::equal(const BlockConstIter &other) const {
    return buf_==other.buf_ && buf_len_==other.buf_len_;
}


std::string_view BlockConstIter::key() const {
    assert(buf_!=container_->data_end_);

    if(!cur_key_.empty())
        return cur_key_;

    if(!last_key_) {
        auto it=BlockConstIter(container_,container_->data_.data()+restarts_block_idx_,restarts_block_idx_);
        while(it.buf_<buf_)
            it++;

        assert(it.buf_==buf_);
    }

    cur_key_.reserve(shared_key_len_+unshared_key_len_);
    cur_key_.append(last_key_,shared_key_len_);
    cur_key_.append(buf_,unshared_key_len_);

    return cur_key_;
}

std::string_view BlockConstIter::value() const {
    return {buf_+unshared_key_len_,static_cast<std::string_view::size_type>(value_len_)};
}