#include "image.hpp"

#include <iostream>

namespace AstralAir
{

namespace Image
{

Image::Image(const ImageMetaData &meta_data, std::vector<char> &&pixels)
    : meta_data_(meta_data), pixels_(std::move(pixels))
{
}

void Image::PrintImageMetaData() const
{
  auto print_all = [](const auto &...params) { ((std::cout << params << "\n"), ...); };
  print_all(meta_data_.unpacked_size, meta_data_.header_size, meta_data_.type, meta_data_.width,
            meta_data_.height, meta_data_.offset_x, meta_data_.offset_y, meta_data_.bpp);
}

const Image::ImageMetaData &Image::GetMetaData() const { return meta_data_; }

const std::vector<char> &Image::GetPixels() const { return pixels_; }
} // namespace Image

} // namespace AstralAir
