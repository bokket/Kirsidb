#include "table_cache.h"

using namespace bokket;


static void DeleteEntry(std::string_view key,std::any value)
{
    auto tf=std::any_cast<TableAndFile*>(value);
    delete tf->table;
    // delete tf->file;
    delete tf;
}

TableCache::TableCache(std::string_view dbname,size_t capacity)
                      :dbname_{dbname}
                      ,cache_{std::make_unique<LRUCachePolicy>(capacity)}
{}



Status TableCache::Get(uint64_t file_number,uint64_t file_size,std::string_view key,std::string& result)
{
    LRUCachePolicy::LRUHandle* handle=nullptr;

    auto s=FindTable(file_number, file_size,&handle);

    if(s.isOK())
    {
        auto tf=std::any_cast<TableAndFile*>(cache_->Value(handle));
        auto table=tf->table;
        s=table->find(key,result);
        
        delete table;
        //delete tf->file;
        delete tf;

        //cache_->Release(handle);
    }

    return s;
}

Status TableCache::FindTable(uint64_t file_number,uint64_t file_size,LRUCachePolicy::LRUHandle** handler)
{
    Status s;
    char buf[sizeof file_number];
    EncodeFixed64(buf, file_number);

    std::string_view key{buf,sizeof buf};

    *handler=cache_->Lookup(key);

    if(*handler==nullptr)
    {
        auto sst_file_name=SSTableFileName(dbname_, file_number);
        ReadableFile* file=nullptr;

        s=tinyio::fileManager::NewRandomAccessFile(sst_file_name,&file);

        SSTable* sst=nullptr;
        if(s.isOK()) {
            sst=SSTable::Open(file, file_size);
            if(sst==nullptr)
            {
                delete file;
            }
        }
        if(!s.isOK())
        {
            delete file;
        } else {
            auto tf=new TableAndFile;
            tf->table=sst;
            tf->file=file;

            *handler=cache_->Insert(key, tf);
            // *handler=cache_->Insert(key, tf,DeleteEntry);
        }
    }
    return s;
}