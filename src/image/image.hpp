#pragma once

#include <cstdint>

namespace AstralAir
{

namespace Image
{

class Image
{
private:
  struct ImageMetaData
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
  ImageMetaData meta_data_;

  Image();
};
} // namespace Image
} // namespace AstralAir
