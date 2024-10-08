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

Status BlockHandle::DecodeFrom(const char *input, BlockHandle &handle) {
    handle.offset_= DecodeFixed64(input);
    handle.size_= DecodeFixed64(input+8);
    return Status::OK();
}

std::string BlockHandle::DebugString(const BlockHandle &handle) {
    return "[offset_: offset = "+
            std::to_string(handle.offset_) +
            ", size = "+std::to_string(handle.size_)+ " ]";
}

void BlockHandle::DebugString() {
    LOG_INFO( "[offset_: offset = {} , size = {} ]" ,
              std::to_string(offset_)  ,
              std::to_string(size_));
}