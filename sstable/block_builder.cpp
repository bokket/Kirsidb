//
// Created by bokket on 2024/2/1.
//

#include "block_builder.h"

using namespace bokket;

BlockBuilder::BlockBuilder()
                         :finished_(false)
{}

void BlockBuilder::add(std::string_view key, std::string_view value) {
//    if (key.empty())
//        return DB::OK;
    assert(!finished_);

    bool need = false;

    //RESTART_INTERVAL

    if (record_num_ % 2== 0) {
        need = true;
        LOG_INFO("restart_size:{}", restart_size());
        restarts_.emplace_back(static_cast<int32_t>(data_.size()));
    }

    if (need) {
        PutFixed32(data_, 0);
        PutFixed32(data_, static_cast<int32_t>(key.size()));

        PutFixed32(data_, static_cast<int32_t>(value.size()));

        LOG_INFO("{}",value.size());

        data_.append(key);
        data_.append(value);
    } else {
        auto min_len = std::min(key.size(), pre_key_.size());
        int shared_key_len = 0;
        for (int i = 0; i < min_len; ++i) {
            if (key[i] != pre_key_[i])
                break;

            ++shared_key_len;
        }
        int unshared_key_len = static_cast<int>(key.size()) - shared_key_len;


        PutFixed32(data_, shared_key_len);

        PutFixed32(data_, unshared_key_len);

        PutFixed32(data_, static_cast<int32_t>(value.size()));

        auto s=key.substr(shared_key_len);
        LOG_INFO("key:{},sub:{},{}",key,s,shared_key_len);

        data_.append(key.substr(shared_key_len));
        data_.append(value);
    }


    ++record_num_;
    pre_key_ = key;

    auto shared_key_len_=DecodeFixed32(data_.data());
    auto unshared_key_len_= DecodeFixed32(data_.data()+sizeof(int32_t));
    auto value_len_= DecodeFixed32(data_.data()+sizeof(int32_t)*2);

    LOG_INFO("{},{},{}",shared_key_len_,unshared_key_len_,value_len_);


    LOG_INFO("{},{}",pre_key_,data_);

    LOG_INFO("record_:{}",record_num_);
    //return DB::OK;
}

Status BlockBuilder::finish() {
    //int restart_point_num=static_cast<int>((record_num_-0.5)/16)+1;
    int restart_point_num=static_cast<int>(restarts_.size());
    int last_offset=static_cast<int>(data_.size());

    LOG_INFO("restarts:{}",restarts_.size());
    LOG_INFO("last_offset:{}",last_offset);

    for(int i=0;i<restarts_.size();++i) {
        int32_t record_num;
        BlockHandle offsetInfo{
            0,
            static_cast<uint64_t>(restarts_[i])
        };
        if(i!=restarts_.size()-1) {
            offsetInfo.size_=restarts_[i+1]-restarts_[i];
            record_num=RESTART_INTERVAL;
        }else {
            offsetInfo.size_=last_offset-restarts_[i];
            record_num=record_num_%RESTART_INTERVAL;
        }
        PutFixed32(data_,record_num);
        PutFixed32(data_,*reinterpret_cast<int64_t*>(&offsetInfo));
    }
    LOG_INFO("restart_point_num:{}",restart_point_num);
    PutFixed32(data_,restart_point_num);
    return Status::OK();
}

Status BlockBuilder::final(std::string& result) {
    int restarts_len=static_cast<int>(restarts_.size());

    for(int i=0;i<restarts_len;++i) {
        PutFixed32(data_,restarts_[i]);
        LOG_INFO("len:{},{}",restarts_len,restarts_[i]);
    }

    PutFixed32(data_,restarts_len);
    result=std::move(data_);

    return Status::OK();
}

std::string_view BlockBuilder::final() {
    if(finished_) {
        return std::string_view {data_};
    }

    int restarts_len=static_cast<int>(restarts_.size());

    for(int i=0;i<restarts_len;++i) {
        PutFixed32(data_,restarts_[i]);
        LOG_INFO("len:{},{}",restarts_len,restarts_[i]);
    }

    PutFixed32(data_,restarts_len);

    finished_= true;
    return std::string_view {data_};
}

void BlockBuilder::clear() {
    finished_= false;

    restarts_.clear();
    pre_key_.clear();

    data_.clear();

    record_num_=0;
}

bool BlockBuilder::empty() const
{
    return data_.empty();
}