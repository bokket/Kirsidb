#include "wal_writer.h"

using namespace bokket::wal;

void Writer::Open(std::string_view path,tinyio::access_mode access_mode)
{
    file_.open(path, access_mode);

    file_.seekg(0);

    //32*1024
    rep_.reserve(kBlockSize);
}

void Writer::Close()
{
    file_.close();
    block_offset_=0;
    size_=0;
}

void Writer::Sync()
{
    file_.sync();
}

void Writer::AppendRecordToBuffer(std::span<const uint8_t> data)
{

    
    std::println("{}",std::string{data.begin(),data.end()});

    auto size=kHeaderSize+data.size();

    size_+=size;

    std::println("{}",size_);

    if(block_offset_+size>kBlockSize) {
        Flush();
        AppendRecord(data);
        return;
    }

    LogRecord record{
        .type=RecordType::kFullType,
    };

    
    record.size=static_cast<uint16_t>(data.size());

    Crc32 crc32;

    //size=7-4=3
    crc32.Extend(&record.size, kHeaderSize-sizeof(record.checksum));
    crc32.Extend(data.data(), data.size());
    record.checksum=crc32.End();


    auto raw_size=rep_.size();
    rep_.resize(raw_size+kHeaderSize);

    LOG_INFO("{}",rep_.size());
    // rep_ + record(7)
    std::memcpy(&rep_[raw_size],&record,kHeaderSize);

    LOG_INFO("{} | {}",rep_,record);

    raw_size=rep_.size();
    if(!data.empty()) {
        rep_.resize(raw_size+data.size());
        // rep_ + record(7) + data.data()
        std::memcpy(&rep_[raw_size],data.data(),data.size());
    }
    block_offset_+=size;

    LOG_INFO("{}",record);
}

void Writer::AppendRecordToBuffer(std::string_view data)
{
    AppendRecordToBuffer( 
        {reinterpret_cast<const uint8_t*>(data.data()),data.size() } 
    );
}

void Writer::Flush()
{
    if(!rep_.empty()) {
        LOG_INFO("{}",std::string(rep_.begin(),rep_.end()));
        file_.write(rep_.data(),rep_.size());
        rep_.resize(0);
    }
}

void Writer::AppendRecord(std::span<const uint8_t> data)
{
    assert(block_offset_<=kBlockSize);

    auto ptr=data.data();
    auto size=data.size();

    bool begin=true;

    do {
        const size_t leftover=kBlockSize-block_offset_;

        if(leftover<kHeaderSize) {
            if(leftover>0) {
                file_.write(kBlockPadding,leftover);
            }

            block_offset_=0;
        }

        const auto avail=kBlockSize-block_offset_-kHeaderSize;
        const auto fragment_size= (size<avail) ? size:avail;

        RecordType type;
        const auto end=(size==fragment_size);

        if(begin&& end) {
            type=RecordType::kFullType;
        } else if(begin) {
            type=RecordType::kFirstType;
        } else if(end) {
            type=RecordType::kLastType;
        } else {
            type=RecordType::kMiddleType;
        }

        WritePhysicalRecord(type, ptr, fragment_size);
        ptr+=fragment_size;
        size-=fragment_size;

        begin=false;


    }while(size>0);
}

void Writer::WritePhysicalRecord(RecordType type,const uint8_t* ptr,size_t size)
{
    assert(size<0xffff);

    assert(block_offset_+kHeaderSize+size<=kBlockSize);

    uint8_t buf[kHeaderSize];

    auto record=reinterpret_cast<LogRecord*>(buf);
    record->type=type;

    record->size=static_cast<uint16_t>(size);

    Crc32 crc32;
    crc32.Extend(&record->size, kHeaderSize-sizeof(record->checksum));
    crc32.Extend(ptr,size);
    record->checksum=crc32.End();
    

    file_.write(buf,kHeaderSize);
    file_.write(ptr,size);
    block_offset_+=kHeaderSize+size;
}