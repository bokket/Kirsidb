#pragma once

#include <span>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string_view>

#include "./crc32.h"

#include "./wal_format.h"

#include "../file/tinyio.h"

#include "../Log/Log.h"

namespace bokket::wal 
{

class Writer
{

private:
    size_t size_{0};
    size_t block_offset_{0};
    tinyio::File file_;
    std::vector<uint8_t> rep_;
public:
    Writer()=default;
    ~Writer()=default;

    void Open(std::string_view path,tinyio::access_mode access_mode);

    void Close();

    void AppendRecordToBuffer(std::span<const uint8_t> data);

    void AppendRecordToBuffer(std::string_view data);

    void Flush();

    void Sync();

    auto& file();

    auto& Size();
private:
    void AppendRecord(std::span<const uint8_t> data);

    void WritePhysicalRecord(RecordType type,const uint8_t* ptr,size_t size);
};     

}