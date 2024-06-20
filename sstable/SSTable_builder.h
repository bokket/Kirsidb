//
// Created by bokket on 24-4-2.
//

#pragma once

#include <boost/crc.hpp>

#include "../file/writableFile.h"
#include "./footer.h"
#include "./block_builder.h"
#include "./Options.h"

namespace bokket
{

class BlockBuilder;

class SSTableBuilder
{
public:

    SSTableBuilder(const Options* options,WritableFile* file);

    Status Add(std::string_view key,std::string_view value);

    Status Finish();

    void FindShortest(std::string& start,std::string_view limit) const;

    [[nodiscard]] size_t NumEntries() const;
private:
    Status flush();

    Status writeBlock(BlockBuilder* block,BlockHandle& handle);

    void writeBytesBlock(std::string_view data,BlockHandle& handle);

private:
    BlockBuilder data_block_;
    BlockBuilder index_block_;

    WritableFile* file_;
    std::string last_key_;

    bool pending_index_entry_;
    BlockHandle pending_handle_;

    size_t num_entries_;
    uint64_t block_offset_;

    const Options* options_;
};

}