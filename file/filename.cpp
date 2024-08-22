#include "filename.h"


namespace bokket
{

static std::string MakeFileName(std::string_view dbname,uint64_t number,const char* suffix)
{
    return std::format("{}/{:06}.{}", dbname, number, suffix);
}

std::string SSTableFileName(std::string_view dbname,uint64_t number)
{
    return MakeFileName(dbname, number, "sst");
}

}