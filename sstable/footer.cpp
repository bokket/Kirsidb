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

    LOG_INFO("Footer:{}",s);

    assert(s.length()<=40);

    s.resize(48);

    return s;
}

DB Footer::DecodeFrom(std::string_view &input,Footer& footer) {
    auto s= BlockHandle::DecodeFrom(input.data(),footer.index_block_);
    if(s==DB::OK) {
        s= BlockHandle::DecodeFrom(input.data(),footer.meta_block_);
    }
    return s;
}

std::string Footer::DebugString() {
    return "[meta_block: offset = " +
           std::to_string(meta_block_.offset_) +
           ", size = " + std::to_string(meta_block_.size_) + " ], " +
           "[index_block: offset = " + std::to_string(index_block_.offset_) +
           ", size = " + std::to_string(index_block_.size_) + " ]";
}