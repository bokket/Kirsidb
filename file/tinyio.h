#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <filesystem>
#include <string_view>
#include <system_error>


#include "./readableFile.h"
#include "./file_writer.h"

#include "../db/status.h"



namespace tinyio
{

namespace fs = std::filesystem;

class fileManager
{

public:
    static bokket::Status NewRandomAccessFile(std::string_view filename,bokket::ReadableFile **result);

};

enum class access_mode
{
    read,
    write,
    sync,
};

class File
{
private:
    int fd_;
public:
    File()=default;
    ~File();

    File(const File&) = delete;
    void operator=(const File&) = delete;

    void open(const fs::path& path,const access_mode& mode);

    [[nodiscard]] bool is_open() const {
        return fd_!=-1;
    }

    //void resize(uint64_t size);

    size_t read(void *buf,size_t size) const;

    size_t read(void *buf,size_t size,std::error_code& error) const;

    size_t write(const void* buf,size_t size) const;

    size_t write(const void* buf,size_t size,std::error_code& error) const;

    void seekg(uint64_t pos,std::error_code& error);

    void seekg(uint64_t pos);

    void sync() const;

    void sync(std::error_code& error) const;
    
    void close();

    [[nodiscard]] uint64_t size() const;

};

}