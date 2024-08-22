#pragma once

#include <cstddef>
#include <optional>
#include <vector>
#include <string_view>

#include "./crc32.h"

#include "./enum_trait.h"
#include "./wal_format.h"
#include "../file/tinyio.h"

namespace bokket::wal 
{



class Reader
{
private:
    tinyio::File file_;
    std::vector<uint8_t> buffer_;

    size_t size_;
    size_t offset_;
    bool eof_;

public:
    Reader();
    ~Reader()=default;

    void Open(std::string_view path);
    std::optional<std::string> ReadRecord();
private:
    const LogRecord* ReadPhysicalRecord();
};    

}