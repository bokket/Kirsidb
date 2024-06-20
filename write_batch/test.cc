

#include <cstddef>
#include <cstdint>
#include <iostream>
int main()
{
      static constexpr size_t kCountSize = sizeof(uint32_t);
  static constexpr size_t kSeqSize = sizeof(uint64_t);
  static constexpr size_t kHeaderSize = kSeqSize + kCountSize;
  

  std::cout<<kCountSize<<" "<<kSeqSize<<" "<<kHeaderSize<<std::endl;
}