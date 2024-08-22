#include "wal_reader.h"

using namespace bokket::wal;

Reader::Reader()
              :eof_{false}
              ,size_{0}
              ,offset_{0}
{}

void Reader::Open(std::string_view path)
{
    file_.open(path, tinyio::access_mode::read);
    buffer_.resize(kBlockSize);
    file_.seekg(0);
}

std::optional<std::string> Reader::ReadRecord()
{
    bool fragmented_record=false;
    std::string res;

    do{
        auto record=ReadPhysicalRecord();

        
        if(record==nullptr) {
            if(eof_==true)
                return std::nullopt;
            else {
                continue;
            }
        }

        switch (record->type) {
            case RecordType::kFullType: {
                if(fragmented_record) {
                    throw std::runtime_error{"Reader::ReadRecord() partial record without kFullType."};
                }
                return std::string{reinterpret_cast<const char*>(record->data),record->size};
            }
            case RecordType::kFirstType: {
                if(fragmented_record) {
                    throw std::runtime_error{"Reader::ReadRecord() partial record without kFirstType."};
                }
                fragmented_record=true;
                res.append(reinterpret_cast<const char*>(record->data),record->size);
                break;
            }
            case bokket::wal::RecordType::kMiddleType: {
                if(fragmented_record==false) {
                     throw std::runtime_error{"Reader::ReadRecord() partial record without kMiddleType."};
                }
                res.append(reinterpret_cast<const char*>(record->data),record->size);
                break;
                }
            case bokket::wal::RecordType::kLastType: {
                if(fragmented_record==false) {
                     throw std::runtime_error{"Reader::ReadRecord() partial record without kLastType."};
                }
                res.append(reinterpret_cast<const char*>(record->data),record->size);
                fragmented_record=false;
                return res;
            }
            default: {
                throw std::runtime_error{"Reader::ReadRecord() partial record without others."};
            }
           
        }
    }while(true);
}

const LogRecord* Reader::ReadPhysicalRecord ()
{
    const LogRecord* record=nullptr;

    do{
        if(size_<kHeaderSize) {
            if(eof_==false) {
                offset_=0;
                size_=file_.read(&buffer_[0],kBlockSize);
                if(size_==0) {
                    eof_=true;
                    return nullptr;
                }else if(size_<kBlockSize) {
                    eof_=true;
                }
                continue;
            }else {
                size_=0;
                return nullptr;
            }
        }

        record=reinterpret_cast<const LogRecord*>(&buffer_[offset_]);

        if(kHeaderSize+record->size > size_) {
            size_=0;
            if(eof_==false) {
                LOG_ERROR("Reader::ReadPhysicalRecord incorrect log record size");
                throw std::runtime_error{"Reader::ReadPhysicalRecord incorrect log record size"};
            }
            return nullptr;
        }


        // if(record->type==RecordType::kZeroType && record->size==0) {
        //     size_=0;
        //     return nullptr;
        // }

        if(record->checksum) {
            Crc32 crc32;
            crc32.Extend(&record->size, kHeaderSize-sizeof(record->checksum));

            crc32.Extend(record->data, record->size);

            if(record->checksum!=crc32.End()) {
                return nullptr;
            }
        }

        offset_+=kHeaderSize+record->size;
        size_-=kHeaderSize+record->size;

        LOG_INFO("{}",*record);

        return record;
    }while(true);

    return record;
}

