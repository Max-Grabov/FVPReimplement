#pragma once 

#include <concepts>
#include <vector>
#include <cstddef>
#include <cstdint>

namespace AstralAir
{

namespace Image
{

class HZC_Format
{
private:
  std::vector<std::byte> stream_;
  struct MetaData
  {
    uint16_t type;
    uint16_t width;
    uint16_t height;
    uint16_t offset_x;
    uint16_t offset_y;
    uint32_t unpacked_size;
    uint32_t header_size; 
  };   

public:
  template<typename T> requires std::convertible_to<std::vector<std::byte>, T>
  explicit HZC_Format(T &&stream) : stream_(std::forward<T>(stream)) {} 
  
  void OpenAndReadMetaData();
};


}
}
