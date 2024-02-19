//
// Created by bokket on 2024/2/2.
//

#include "block.h"


using namespace bokket;

Block::Iterator &Block::Iterator::operator++() {
    if(!Valid())
        //FetchWithoutValue();

    valid_= false;
}

DB Block::Init(std::string_view data) {
    data_=data;
    const char* buf=data_.data();
    size_=data_.size();

    size_t restarts_len_offset=size_- sizeof(int32_t);
    int32_t restarts_len=DecodeFixed32(buf+restarts_len_offset);

    restarts_offset_=restarts_len_offset-restarts_len*sizeof(int32_t);
    data_buf_=data.substr(0,restarts_offset_);

    for(int i=0;i<restarts_len;++i) {
        int32_t offset= DecodeFixed32(buf+restarts_offset_+i* sizeof(int32_t));
        restarts_.emplace_back(offset);
    }


    LOG_INFO("Block::Init() restarts_offset_:{}",restarts_offset_);
    LOG_INFO("Block::Init() data_:{}",data_);
    LOG_INFO("Block::Init() data_buf_:{}",data_buf_);
    return DB::OK;
}


std::vector<std::string_view> Block::Get(std::string_view key) {
//    int32_t restarts_len=restarts_.size();
//    size_t key_len=key.size();
    std::vector<std::string_view> result;

    auto index= Seek(key);
    LOG_INFO("index:{}",index);
    if(index==-1) {
        const char* restart_record=data_.data()+restarts_[0];
        std::string_view key;
        std::string_view value;

        auto state=DecodeEntry(restart_record,key,value);
        if(state!=DB::OK) {
            LOG_ERROR("DecodeEntry error:{}", Status(state));
            return result;
        }
        result.emplace_back(value);
        return result;
    }

    auto value=ParseKey(index,key);
    return value;
}

std::vector<std::string_view> Block::ParseKey(int index, std::string_view& want) {
    int i=0;
    auto cur_entry=data_.data()+restarts_[index];
    std::string key;
    std::string_view pre_key;
    std::vector<std::string_view> result;
    while(i<RESTART_INTERVAL and cur_entry<data_buf_.end()) {
        int32_t shared_key_len=DecodeFixed32(cur_entry);
        int32_t unshared_key_len=DecodeFixed32(cur_entry+sizeof(int32_t));
        int32_t value_len= DecodeFixed32(cur_entry+sizeof(int32_t)*2);

        if(shared_key_len!=0)
            key=pre_key.substr(0,shared_key_len);

        key.append(cur_entry+ sizeof(int32_t)*3,static_cast<std::string_view::size_type>(unshared_key_len));


        LOG_INFO("shared_key_len:{} | unshared_key_len:{} | value_len:{}",shared_key_len,unshared_key_len,value_len);
        LOG_INFO("cur_entry:{}",cur_entry);
        LOG_INFO("key:{}",key);
        LOG_INFO("pre_key:{}",pre_key);

        if(Compare(key,want)>=0) {
            std::string_view value={cur_entry+sizeof(int32_t)*3+unshared_key_len,static_cast<std::string_view::size_type>(value_len)};
            LOG_INFO("{}",value);
            //return value;
            result.emplace_back(value);
            cur_entry+=sizeof(int32_t)*3+unshared_key_len+value_len;
        } else {
            cur_entry+=sizeof(int32_t)*3+unshared_key_len+value_len;
        }
        ++i;
        pre_key=std::move(key);
    }

    if(i==RESTART_INTERVAL and cur_entry<data_buf_.end()) {
        std::string_view key;
        std::string_view value;
        auto state=DecodeEntry(cur_entry,key,value);
        if(state!=DB::OK) {
            LOG_ERROR("DecodeEntry error:{}", Status(state));
            return result;
        }
        LOG_INFO("value:{}",value);
        result.emplace_back(value);
    }

    return result;
}

//void Block::Gets(std::string_view key) {
//    size_t restarts_size=restarts_.size();
//    int32_t left=0;
//    int32_t right=restarts_size-1;
//
//    std::string_view restarts_key;
//    std::string_view restarts_value;
//
//    while(left<right) {
//        int32_t mid=(left+right+1)>>1;
//        const char* restart_record=data_.data()+restarts_[mid];
//        LOG_INFO("{}",restarts_[mid]);
//
//        auto state= DecodeEntry(restart_record,restarts_key,restarts_value);
//        LOG_INFO("{},{}",restarts_key,restarts_value);
//        if(state!=DB::OK) {
//            LOG_ERROR("DecodeEntry error:{}", Status(state));
//            return;
//        }
//
//        if(Compare(restarts_key,key)<0) {
//            left=mid;
//        } else {
//            right=mid-1;
//        }
//
//        LOG_INFO("left:{},right:{}",left,right);
//    }
//}

int Block::Seek(std::string_view key) {
    size_t restarts=restarts_.size();
    int left=0;
    int right=restarts-1;

    std::string_view restarts_key;
    std::string_view restarts_value;

    LOG_INFO("left:{},right:{}",left,right);

    while(left<=right) {
        int mid=(left+right)>>1;
        const char* restart_record=data_.data()+restarts_[mid];
        LOG_INFO("{}",restarts_[mid]);

        auto state= DecodeEntry(restart_record,restarts_key,restarts_value);
        LOG_INFO("{},{}",restarts_key,restarts_value);
        if(state!=DB::OK) {
            LOG_ERROR("DecodeEntry error:{}", Status(state));
            return -1;
        }

        if(Compare(restarts_key,key)<0) {
            left=mid+1;
        } else {
            right=mid-1;
        }

        LOG_INFO("left:{},right:{}",left,right);
    }

    //size_t restarts=restarts_.size();
    //    int left=0;
    //    int right=restarts-1;

    int index={0};
    if(left!=restarts) {
        const char* restart_record=data_.data()+restarts_[left];
        auto state= DecodeEntry(restart_record,restarts_key,restarts_value);
        if(state!=DB::OK) {
            LOG_ERROR("DecodeEntry error:{}", Status(state));
            return -1;
        }
        LOG_INFO("{},{}",restarts_key,restarts_value);
        if(!Compare(restarts_key,key))  {
            index=left;
            return index;
        }
    }
    index=right;
    return index;
}

DB Block::DecodeEntry(const char *restart_cord, std::string_view &restarts_key,std::string_view &restarts_value) {
    int32_t shared_key_len=DecodeFixed32(restart_cord);
    if(shared_key_len!=0)
        return DB::Unsupported;

    int32_t unshared_key_len=DecodeFixed32(restart_cord+sizeof(int32_t));

    int32_t value_len= DecodeFixed32(restart_cord+sizeof(int32_t)*2);

    restarts_key={restart_cord+ sizeof(int32_t)*3,static_cast<std::string_view::size_type>(unshared_key_len)};
    restarts_value={restart_cord+sizeof(int32_t)*3+unshared_key_len,static_cast<std::string_view::size_type>(value_len)};

    return DB::OK;
}


void Block::Iterator::iter() {
    if(cur_entry_ >= container_->data_buf_.end() or
       entries_idx_>=RESTART_INTERVAL or
       restarts_block_idx_>=container_->restarts_.size())
        return;

    shared_key_len_=DecodeFixed32(cur_entry_);
    unshared_key_len_= DecodeFixed32(cur_entry_+ sizeof(int32_t));
    value_len_= DecodeFixed32(cur_entry_+sizeof(int32_t)*2);

    if(cur_key_.size() >= shared_key_len_)
        cur_key_=cur_key_.substr(0,shared_key_len_);

    cur_key_.append(cur_entry_+sizeof(int32_t)*3,unshared_key_len_);
    cur_value_.append(cur_entry_+ sizeof(int32_t)*3+unshared_key_len_,value_len_);


    valid_= true;
}
