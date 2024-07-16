//
// Created by bokket on 2024/2/1.
//

#include "coding.h"
#include <cstddef>
#include <cstdint>
#include "Buf.h"

namespace bokket
{

/*
  |----------------------------------------------------------------|
  |     位数	     |   31-24    |   23-16   |	  15-8	  |    7-0     |
  |----------------------------------------------------------------|
  |     val	     |  01001000  |	 10110111 |	01100101  |	11010010   |
  |----------------------------------------------------------------|
  |    0xff      |	00000000  |	 00000000 |	00000000  |	11111111   |
  |----------------------------------------------------------------|
  |  val & 0xff  |	00000000  |	 00000000 |	00000000  |	11010010   |
  |----------------------------------------------------------------|
*/


void EncodeFixed8(char *buf, uint8_t val) {
    buf[0] = val & 0xff;
}

void EncodeFixed32(char *buf, uint32_t val) {
    buf[0] = val & 0xff;
    buf[1] = (val >> 8) & 0xff;
    buf[2] = (val >> 16) & 0xff;
    buf[3] = (val >> 24) & 0xff;
}

void EncodeFixed64(char *buf, uint64_t val) {
    buf[0] = val & 0xff;
    buf[1] = (val >> 8) & 0xff;
    buf[2] = (val >> 16) & 0xff;
    buf[3] = (val >> 24) & 0xff;
    buf[4] = (val >> 32) & 0xff;
    buf[5] = (val >> 40) & 0xff;
    buf[6] = (val >> 48) & 0xff;
    buf[7] = (val >> 56) & 0xff;
}

uint8_t DecodeFixed8(const char *data) {
    return *reinterpret_cast<const uint8_t*>(data);
}

uint32_t DecodeFixed32(const char *data) {
    auto _data = reinterpret_cast<const uint8_t *>(data);
    return static_cast<uint32_t>(_data[0]) |
           (static_cast<uint32_t>(_data[1]) << 8) |
           (static_cast<uint32_t>(_data[2]) << 16) |
           (static_cast<uint32_t>(_data[3]) << 24);
}

uint64_t DecodeFixed64(const char *data) {
    auto _data = reinterpret_cast<const uint8_t *>(data);
    return static_cast<uint64_t>(_data[0]) |
           (static_cast<uint64_t>(_data[1]) << 8) |
           (static_cast<uint64_t>(_data[2]) << 16) |
           (static_cast<uint64_t>(_data[3]) << 24) |
           (static_cast<uint64_t>(_data[4]) << 32) |
           (static_cast<uint64_t>(_data[5]) << 40) |
           (static_cast<uint64_t>(_data[6]) << 48) |
           (static_cast<uint64_t>(_data[7]) << 56);
}

}


namespace varint 
{

int VarintLength(uint64_t v)
{
    int len=1;

    // v>=128
    while(v>=(1 << 7))
    {
        v>>=7;
        std::cout<<v<<std::endl;
        ++len;
    }
    return len;
}

void printBinary(uint8_t n) {
    printf("Binary representation of uint8_t %u is: ", n);
    for(int i = 7; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
    }
    printf("\n");
}

char* EncodeVarint32(char* dst,uint32_t v)
{
    auto ptr=reinterpret_cast<uint8_t*>(dst);

    //128
    static const int B=(1<<7);

    if(v < (1<<7))
    {
        *(ptr++)=v;
        //std::cout<<std::bitset<sizeof(ptr)>(ptr);
    }else if(v<(1<<14))
    {
        *(ptr++)=v | B;
        std::cout<<"?";
        //std::cout<<std::bitset<sizeof(ptr)>(ptr);
        *(ptr++)=v>>7;
    }else if(v<(1<<21))
    {
        *(ptr++)=v | B;
        *(ptr++)=(v>>7) | B;
        *(ptr++)=v>>14;
    }else if(v<(1<<28))
    {
        *(ptr++)=v | B;
        *(ptr++)=(v>>7) | B;
        *(ptr++)=(v>>14) | B;
        *(ptr++)=v>>21;
    } else 
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14) | B;
        *(ptr++) = (v >> 21) | B;
        *(ptr++) = v >> 28;
    }
    
    return reinterpret_cast<char*>(ptr);
}

void AppendVar32(std::string* dst,uint32_t v)
{
    char buf[5];
    char* ptr=EncodeVarint32(buf, v);

    dst->append(buf,ptr-buf);
}

void AppendVarString(std::string* res,const bokket::Buf& buf)
{
    AppendVar32(res, static_cast<uint32_t>(buf.size()));
    res->append(buf.data(),buf.size());
}

const char* GetVarint32Ptr(const char* p, const char* limit,
                                  uint32_t* value) {
  if (p < limit) {
    uint32_t result = *(reinterpret_cast<const uint8_t*>(p));
    if ((result & 128) == 0) {
      *value = result;
      return p + 1;
    }
  }
  return GetVarint32PtrFallback(p, limit, value);
}


const char* GetVarint32PtrFallback(const char* p, const char* limit,
                                   uint32_t* value) {
  uint32_t result = 0;
  for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
    uint32_t byte = *(reinterpret_cast<const uint8_t*>(p));
    p++;
    if (byte & 128) {
      // More bytes are present
      result |= ((byte & 127) << shift);
    } else {
      result |= (byte << shift);
      *value = result;
      return reinterpret_cast<const char*>(p);
    }
  }
  return nullptr;
}


bool GetVarint32(bokket::Buf* input,uint32_t* value)
{
    const char* p=input->data();
    const char* limit=p+input->size();

    const char* q=GetVarint32Ptr(p, limit, value);

    if(q==nullptr)
    {
        return false;
    }else {
        *input=bokket::Buf(q,limit-q);
        return true;
    }
}

bool GetLengthPrefixedString(bokket::Buf* input,bokket::Buf* result)
{
    uint32_t len;
    if(GetVarint32(input, &len) && input->size()>=len)
    {
        *result=bokket::Buf(input->data(),len);

        input->skip(len);

        return true;
    } else {
        return false;
    }
}

}
