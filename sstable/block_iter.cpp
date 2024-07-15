//
// Created by bokket on 2024/2/20.
//

#include "block_iter.h"
#include <algorithm>
#include <iterator>

using namespace bokket;

Status Block::Init(std::string_view content) {
    data_ = content;
    size_ = content.size();

    size_t restarts_len_offset = size_ - sizeof(int32_t);

    LOG_INFO("{}", std::string_view {data_.data() + restarts_len_offset, 4});
    num_restart_ = DecodeFixed32(data_.data() + restarts_len_offset);

    assert(size_ >= 4 * (num_restart_ + 1));

    data_end_ = data_.data() + size_ - 4 * (num_restart_ + 1);

    LOG_INFO("block_iter size_:{},num_restart_:{},data_:{}", size_, num_restart_,
             std::string_view {data_.data(), size_ - 4 * (num_restart_ + 1)});

    auto shared_key_len_ = DecodeFixed32(data_.data());
    auto unshared_key_len_ = DecodeFixed32(data_.data() + sizeof(int32_t));
    auto value_len_ = DecodeFixed32(data_.data() + sizeof(int32_t) * 2);

    LOG_INFO("{},{},{}", shared_key_len_, unshared_key_len_, value_len_);

    LOG_INFO("{}", data_);

    return Status::OK();
}

Block::Block(const BlockContent& content) : data_(content.data), size_(content.data.size()) {
    size_t restarts_len_offset = size_ - sizeof(int32_t);

    LOG_INFO("{}", std::string_view {data_.data() + restarts_len_offset, 4});
    num_restart_ = DecodeFixed32(data_.data() + restarts_len_offset);

    assert(size_ >= 4 * (num_restart_ + 1));

    data_end_ = data_.data() + size_ - 4 * (num_restart_ + 1);

    LOG_INFO("block_iter size_:{},num_restart_:{},data_:{}", size_, num_restart_,
             std::string_view {data_.data(), size_ - 4 * (num_restart_ + 1)});
}

Block::Block(std::string_view content) : data_(content), size_(content.size()) {
    size_t restarts_len_offset = size_ - sizeof(int32_t);

    LOG_INFO("{}", std::string_view {data_.data() + restarts_len_offset, 4});
    num_restart_ = DecodeFixed32(data_.data() + restarts_len_offset);

    assert(size_ >= 4 * (num_restart_ + 1));

    data_end_ = data_.data() + size_ - 4 * (num_restart_ + 1);

    LOG_INFO("block_iter size_:{},num_restart_:{},data_:{}", size_, num_restart_,
             std::string_view {data_.data(), size_ - 4 * (num_restart_ + 1)});
}

Block::Block(const std::string& content) : data_(content), size_(content.size()) {
    size_t restarts_len_offset = size_ - sizeof(int32_t);

    LOG_INFO("{}", std::string_view {data_.data() + restarts_len_offset, 4});
    num_restart_ = DecodeFixed32(data_.data() + restarts_len_offset);

    assert(size_ >= 4 * (num_restart_ + 1));

    data_end_ = data_.data() + size_ - 4 * (num_restart_ + 1);

    LOG_INFO("block_iter size_:{},num_restart_:{},data_:{}", size_, num_restart_,
             std::string_view {data_.data(), size_ - 4 * (num_restart_ + 1)});
}

Block::Iter Block::find(std::string_view key) const {
    auto it = lower_bound(key);
    return Compare(it.key(), key) != 0 ? end() : it;
}

Block::Iter Block::find_if(std::string_view key) const {
    for(auto it=begin();it!=end();it++)
    {
        if(it.key()==key)
            return it;
    }
    return end();
}

Block::Iter Block::begin() const {
    return {this, data_.data(), 0};
}

Block::Iter Block::end() const {
    return {this, data_end_, 0};
}

uint32_t Block::restartPoint(int id) const {
    assert(id <= num_restart_);
    uint32_t restart_len = size_ - 4 * (num_restart_ - id + 1);
    uint32_t r = DecodeFixed32(data_.data() + restart_len);

    assert(r <= data_end_ - data_.data());

    return r;
}

std::string_view Block::keyAtRestartPoint(int id) const {
    uint32_t shared, unshared, value_len;

    uint32_t pos = restartPoint(id);

    LOG_INFO("keyAt:{} id:{}", pos, id);

    std::string_view buf {data_.data() + pos,
                          static_cast<std::string_view::size_type>(data_end_ - data_.data() - pos)};
    shared = DecodeFixed32(buf.data());
    assert(shared == 0);

    unshared = DecodeFixed32(buf.data() + sizeof(int32_t));
    value_len = DecodeFixed32(buf.data() + sizeof(int32_t) * 2);

    LOG_INFO("buf:{} shared:{} unshared:{} value_len:{} | {}", buf, shared, unshared, value_len,
             std::string_view {buf.data() + sizeof(int32_t) * 3,
                               static_cast<std::string_view::size_type>(unshared)});

    return std::string_view {buf.data() + sizeof(int32_t) * 3,
                             static_cast<std::string_view::size_type>(unshared)};
}

Block::Iter Block::lower_bound(std::string_view key) const {
    int left = 0, mid = left;
    int right = num_restart_;

    while (left + 1 < right) {
        mid = (left + right) >> 1;

        if (Compare( keyAtRestartPoint(mid),key) >= 0) {
            right = mid;
            //LOG_INFO("{}",Compare(keyAtRestartPoint(mid), key));
        } else {
            left = mid;
            //LOG_INFO("{} | {}",keyAtRestartPoint(mid),key);
            //LOG_INFO("{}",Compare(keyAtRestartPoint(mid), key));
        }
    }

    if (Compare( keyAtRestartPoint(left),key) > 0) {
        //assert(left == 0 && right == left + 1);
        return end();
    }

    uint32_t pos = restartPoint(left);

    LOG_INFO("left:{} pos:{}", left, pos);

    auto it = Iter(this, data_.data() + pos, pos);

    for (; it != end(); it++) {
        //     LOG_INFO("it.key:{} | key:{}",it.key(),key);
        auto res = Compare(it.key(), key);

        LOG_INFO("res:{} it.key():{} it.value:{}", res, it.key(), it.value());
        if (res >= 0) break;
    }
    return it;
}

BlockConstIter::BlockConstIter(const Block* container, const char* p, uint32_t restart)
                              : container_ {container}, restarts_block_idx_(restart) {
    init(p);
}

void BlockConstIter::increment() {
    std::swap(last_key_, cur_key_);

    LOG_INFO("increment last_key_:{}", last_key_);

    std::string_view buf {buf_.data() + unshared_key_len_ + value_len_,
                          buf_len_ - unshared_key_len_ - value_len_ - shared_key_len_};
    LOG_INFO("increment:{} | {} | cur_len:{}", buf_, shared_key_len_,
             buf_len_ - unshared_key_len_ - value_len_);
    buf_ = buf;

    cur_key_.clear();
    init(buf.data());
}

bool BlockConstIter::equal(const BlockConstIter& other) const {
    return buf_ == other.buf_ && buf_len_ == other.buf_len_;
}

void BlockConstIter::init(const char* p) {
    auto len = container_->data_end_ - p;

    assert(len >= 0);

    if (len > 0) {
        shared_key_len_ = DecodeFixed32(p);
        unshared_key_len_ = DecodeFixed32(p + sizeof(int32_t));
        value_len_ = DecodeFixed32(p + sizeof(int32_t) * 2);

        buf_ = std::string_view {p + sizeof(int32_t) * 3, static_cast<std::string_view::size_type>(
                                                                  len - sizeof(int32_t) * 3)};

        buf_len_ = len - sizeof(int32_t) * 3;

        LOG_INFO("cur_entry:{}", cur_entry_);

        cur_entry_ = std::string_view {
                p + sizeof(int32_t) * 3,
                static_cast<std::string_view::size_type>(unshared_key_len_ + value_len_)};

        LOG_INFO("len:{} | shared_key_len_:{} | unshared_key_len_:{} | value_len_:{} |", len,
                 shared_key_len_, unshared_key_len_, value_len_);

        //cur_key_
        if (!last_key_.empty()) {
            LOG_INFO("last_key:{}| cur_entry:{} | cur_key:{}", last_key_, cur_entry_, cur_key_);
        }

        assert(cur_key_.empty());

        cur_key_.reserve(shared_key_len_ + unshared_key_len_);
        cur_key_.append(last_key_.data(), shared_key_len_);
        cur_key_.append(cur_entry_.data(), unshared_key_len_);

        if (unshared_key_len_ == 0) {
            restarts_block_idx_ = static_cast<int32_t>(p - container_->data_.data());
            LOG_INFO("restart_index:{}", restarts_block_idx_);
        }
    } else {
        // buf_=p;
        buf_ = {};
        buf_len_ = 0;
    }
}

std::string_view BlockConstIter::key() const {
    assert(buf_.data() != container_->data_end_);

    if (!cur_key_.empty()) 
        return cur_key_;

    return cur_key_;
}

std::string_view BlockConstIter::value() const {
    return {cur_entry_.data() + unshared_key_len_,
            static_cast<std::string_view::size_type>(value_len_)};
}

BlockConstIter::BlockConstIter()
                            : buf_ {},
                            last_key_ {},
                            cur_key_ {},
                            cur_entry_ {},
                            restarts_block_idx_ {0},
                            buf_len_ {0},
                            container_ {nullptr},
                            value_len_ {0},
                            unshared_key_len_ {0},
                            shared_key_len_ {0} {}

BlockConstIter::BlockConstIter(const BlockConstIter& rhs)
                            : buf_ {rhs.buf_},
                            last_key_ {rhs.last_key_},
                            restarts_block_idx_ {rhs.restarts_block_idx_},
                            buf_len_ {rhs.buf_len_},
                            container_ {rhs.container_},
                            value_len_ {rhs.value_len_},
                            unshared_key_len_ {rhs.unshared_key_len_},
                            shared_key_len_ {rhs.shared_key_len_},
                            cur_key_ {rhs.cur_key_},
                            cur_entry_ {rhs.cur_entry_} {
}

BlockConstIter& BlockConstIter::operator=(const BlockConstIter& rhs) {
    
    container_ = rhs.container_;
    restarts_block_idx_ = rhs.restarts_block_idx_;
    buf_ = rhs.buf_;
    last_key_ = rhs.last_key_;
    buf_len_ = rhs.buf_len_;
    value_len_ = rhs.value_len_;
    unshared_key_len_ = rhs.unshared_key_len_;
    shared_key_len_ = rhs.shared_key_len_;
    cur_entry_ = rhs.cur_entry_;
    cur_key_ = rhs.cur_key_;

    return *this;
}