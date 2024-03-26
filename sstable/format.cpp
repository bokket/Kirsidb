//
// Created by bokket on 24-3-26.
//

#include "format.h"

#include "coding.h"
using namespace bokket;

void BlockHandle::Encode(std::string &output, const BlockHandle &handle) {
    PutFixed64(output,handle.offset_);
    PutFixed64(output,handle.size_);
}

std::string BlockHandle::EncodeToString() const {
    std::string s;
    PutFixed64(s,offset_);
    PutFixed64(s,size_);
    return s;
}

DB BlockHandle::DecodeFrom(const char *input, bokket::BlockHandle &handle) {
    handle.offset_= DecodeFixed64(input);
    handle.size_= DecodeFixed64(input+8);
    return DB::OK;
}

std::string BlockHandle::DebugString(const bokket::BlockHandle &handle) {
    return "[offset_: offset = "+
            std::to_string(handle.offset_) +
            ", size = "+std::to_string(handle.size_)+ " ]";
}