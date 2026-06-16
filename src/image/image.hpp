#pragma once

#include <cstdint>
#include <vector>

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
    uint16_t bpp;
    uint32_t unpacked_size;
    uint32_t header_size;
  };
  ImageMetaData meta_data_;
  std::vector<char> pixels_;

public:
  Image(const ImageMetaData &, std::vector<char> &&);
  void PrintImageMetaData() const;

  const ImageMetaData &GetMetaData() const;
  const std::vector<char> &GetPixels() const;
};
} // namespace Image
} // namespace AstralAir
