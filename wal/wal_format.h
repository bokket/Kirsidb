#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <format>


#include "./enum_trait.h"

namespace bokket::wal
{


enum class RecordType:uint8_t
{
    kZeroType=0,
    kFullType=1,


    kFirstType=2,
    kMiddleType=3,
    kLastType=4
};

struct LogRecord {
    //4
    uint32_t checksum;
    //2
    uint16_t size;
    //1
    RecordType type;
    //1
    uint8_t data[1];
};


static constexpr size_t kBlockSize=32*1024;

//7
static constexpr size_t kHeaderSize=offsetof(LogRecord, data);

static const char* kBlockPadding="\x00\x00\x00\x00\x00\x00";

}

using namespace bokket::wal;
template<>
struct std::formatter<LogRecord>:std::formatter<std::string>
{
    template<typename FormatContext>
    auto format(const LogRecord& record, FormatContext& ctx)const {
        //auto type=std::to_underlying(record.type);
        //return std::format_to(ctx.out(),
        // "LogRecord(Checksum: {}, Size: '{}', Type:{} , Data:[{}])",
            // record.checksum,
            // record.size,
            // record.type,  // 这会使用 RecordType 的 formatter
            // record.data[0]
        // );
        return std::formatter<std::string>::format(
            std::format("LogRecord(Checksum: {}, Size: '{}', Type:{} , Data:[{}])", 
            record.checksum, record.size,record.type,record.data), ctx
        );
    }
};