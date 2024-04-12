//
// Created by bokket on 2024/1/31.
//

#pragma once


#include <optional>
#include <string_view>
#include <cstdint>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <filesystem>

#include "./writableFile.h"
#include "../db/export.h"
#include "../db/status.h"
#include "../Log/Log.h"
#include "../sstable/block_iter.h"


namespace bokket
{

class RandomAccessFile
{
public:
    RandomAccessFile(std::string_view path)
                    :filename_(path)
    {}

    ~RandomAccessFile() {
        close(fd_);
    }

    Status read(size_t n,size_t offset,char* dst,std::string* result);
private:
    int fd_;
    std::string filename_;
};

class Writer final :public WritableFile
{
private:
    FILE *file_;
    std::string filename_;

public:
    Writer(std::string_view filename);
    ~Writer() override;

    Status append(std::string_view data) override;
    Status flush() override;

    Status sync() override;

    Status close() override;

};

class FileWriter final :public WritableFile
{
private:
    //2^16字节
    //65536字节等于64KB
    //1M等于1024KB，1KB等于1024字节（B）
    static constexpr size_t KBufferSize=6;
    std::string file_path_;

    int fd_;
    bool closed_;

    //std::array<char,KBufferSize> buf_;
    char buf_[KBufferSize];
    size_t pos_;
public:
    //FileWriter(std::string_view filename,int fd);
    FileWriter(std::string_view filename);
    ~FileWriter() override;

    Status append(std::string_view data) override;
    Status flush() override;

    Status sync() override;

    Status close() override;
};

inline Block* ReadBlockFromFile(RandomAccessFile* file,const BlockHandle &handle,Status &s) {
    //assert(handle.size_>)

    std::unique_ptr<char[]> p_block_buf(new char[handle.size_]);

    char* block=p_block_buf.get();

    std::string res;

    s=file->read(handle.size_,handle.offset_-handle.size_,block,&res);

    if(s) {
        if(res.length()<handle.size_)
            s=Status::IOError("ReadBlockFromFile error");
    }

    if(!s)
        return nullptr;

    Block* ret=new Block(res);

    p_block_buf.release();

    s=Status::OK();

    return ret;
}


//class FileWriter
//{
//private:
//    //2^16字节
//    //64KB
//    static constexpr size_t BUFFER_SIZE=65536;
//    size_t buffer_offset=0;
//
//    char buffer[BUFFER_SIZE];
//    int fd_;
//
//public:
//    explicit FileWriter(std::string_view path,bool append=true);
//
//    ~FileWriter();
//
//    DB append(std::optional<const char *> data,size_t len,bool flush=false);
//
//    DB flush();
//
//    void sync();
//
//    void close();
//
//private:
//    DB persist(const char* data,int len);
//
//    bool is_buffer_full() const {
//        return buffer_offset==BUFFER_SIZE;
//    }
//};


}
