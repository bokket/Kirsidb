#pragma once
#include <any>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>


#include "Options.h"

#include "../db/status.h"
#include "../sstable/SSTable.h"
#include "../file/tinyio.h"
#include "../cache/cache.h"

#include "../file/filename.h"

namespace bokket 
{

struct TableAndFile
{
    SSTable* table;
    ReadableFile* file;
};

class TableCache
{
private:
    const std::string dbname_;
    //const Options& option_;


    
    std::unique_ptr<LRUCachePolicy> cache_;

public:
    TableCache(std::string_view dbname,size_t capacity);

    ~TableCache()=default;

    Status Get(uint64_t file_number,uint64_t file_size,std::string_view key,std::string& result);

private:
    Status FindTable(uint64_t file_number,uint64_t file_size,LRUCachePolicy::LRUHandle** handler);


};

}