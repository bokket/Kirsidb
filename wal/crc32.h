#pragma once

#include <cstddef>
#include <cstdint>
namespace bokket 
{

class Crc32
{
public:
    Crc32()=default;
    ~Crc32()=default;

    void Clear();
    
    //https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks#CRC-32_algorithm
    void Extend(const void* buf,size_t size);
    uint32_t End();

private:
    uint32_t value_=0xffffffff;
};

}