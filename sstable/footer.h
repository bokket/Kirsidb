//
// Created by bokket on 24-3-26.
//

#pragma once

#include <string>
#include <string_view>

#include "format.h"

#include "../Log/Log.h"
#include "../db/status.h"

namespace bokket
{



// echo https://github.com/bokket/Kirsidb | sha1sum
static const uint64_t kTableMagicNumber = 0x41ac5b09a8323be3ull;

class Footer
{
public:
    Footer(const BlockHandle& meta,const BlockHandle& index)
          :meta_block_{meta}
          ,index_block_{index}
    {}

    void EncodeTo(std::string& dst);

    std::string EncodeToString() const;

    DB DecodeFrom(std::string_view& input,Footer& footer);

    //void setFilterBlockMetaData();

    std::string DebugString();

private:
    BlockHandle meta_block_;
    BlockHandle index_block_;
};

}


