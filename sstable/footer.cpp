//
// Created by bokket on 24-3-26.
//

#include "footer.h"

using namespace bokket;

void Footer::EncodeTo(std::string &dst) {
  //  block_index_.Encode(dst);
}

std::string Footer::EncodeToString() const {

    std::string s{index_block_.EncodeToString()+meta_block_.EncodeToString()};

    LOG_INFO("Footer:{} {}",s,s.size());

    assert(s.length()<=40);

    //s.resize(48);



    return s;
}

Status Footer::DecodeFrom(std::string_view &input,Footer& footer) {


    auto s= BlockHandle::DecodeFrom(input.data(),footer.index_block_);
    if(!s) [[unlikely]] {
        return Status::Corruption("Footer::DecodeFrom error");
    }

    s= BlockHandle::DecodeFrom(input.data()+16,footer.meta_block_);

    return s;
}

void Footer::DebugString() {
    LOG_INFO( "[meta_block: offset = {}, size = {} ], [index_block: offset = {}, size = {} ]" ,
           std::to_string(meta_block_.offset_) ,std::to_string(meta_block_.size_)  ,
           std::to_string(index_block_.offset_) ,
           std::to_string(index_block_.size_) );

    meta_block_.DebugString();
    index_block_.DebugString();

    return ;
}