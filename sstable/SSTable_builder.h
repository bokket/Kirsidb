//
// Created by bokket on 24-4-2.
//

#pragma once

#include <cstdint>
#include "./footer.h"
#include "./Options.h"
#include "./block_builder.h"

#include "../file/writableFile.h"

namespace bokket {

class BlockBuilder;

class SSTableBuilder {
public:
    SSTableBuilder(const Options* options, WritableFile* file);

    Status Add(std::string_view key, std::string_view value);

    Status Finish();

    [[nodiscard]] size_t NumEntries() const;

    [[nodiscard]] uint64_t fileSize() const;

private:
    Status flush();

    Status writeBlock(BlockBuilder* block, BlockHandle& handle);

    Status writeBlock(BlockBuilder* block);

    void writeBytesBlock(std::string_view data, BlockHandle& handle);

    void FindShortest(std::string& start, std::string_view limit) const;

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

} // namespace bokket