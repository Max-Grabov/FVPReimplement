#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "image/image.hpp"
#include "util/file_view.hpp"

namespace fvp
{

namespace Formats
{

struct SaveInformation
{
  std::vector<std::byte> byte_stream_1;
  std::vector<std::byte> byte_stream_2;
  std::vector<std::byte> preview_text;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t day_of_week;
  uint8_t hour;
  uint8_t minute;
};

// Should be equivalent to _sprintf(buffer, "%s/save/s%03d.bin", save_file_directory, save_number)
// std format will pad the zeroes properly
// std::vformat("{}/save/s{:03}.bin", std::make_format_args(save_file_directory_, save_number))
inline SaveInformation GetSaveInformation(const std::string &path)
{
  Utility::View save(path);
  SaveInformation save_info;
  uint32_t ptr{0};

  save_info.year = save.Read<uint16_t>(ptr);
  ptr += 2;
  save_info.month = save.Read<uint8_t>(ptr++);
  save_info.day = save.Read<uint8_t>(ptr++);
  save_info.day_of_week = save.Read<uint8_t>(ptr++);
  save_info.hour = save.Read<uint8_t>(ptr++);
  save_info.minute = save.Read<uint8_t>(ptr++);

  uint16_t size{};
  size = save.Read<uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    save_info.byte_stream_1 = save.Read(ptr, size + 1);
    save_info.byte_stream_1.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save.Read<uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    save_info.byte_stream_2 = save.Read(ptr, size + 1);
    save_info.byte_stream_2.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save.Read<uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    save_info.preview_text = save.Read(ptr, size + 1);
    save_info.preview_text.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  return save_info;
}

inline Image::Image GetSavePreviewImage(const std::string &path, uint32_t width, uint32_t height)
{
  Utility::View save(path);

  // We read size data until the image data
  // This goes through all the save metadata
  uint32_t ptr{7};
  ptr += (save.Read<uint16_t>(ptr) + 2);
  ptr += (save.Read<uint16_t>(ptr) + 2);
  ptr += (save.Read<uint16_t>(ptr) + 2);

  // Image is raw bytes, so we can just fread and create
  auto pixels = save.Read(ptr, width * height * 4);

  // This is safe as far as I know (narrowing the type). Image metadata as defined in zlib for full
  // screen images fits in uint16_t
  return Image::Image({0, static_cast<uint16_t>(width), static_cast<uint16_t>(height), 0, 0, 0,
                       static_cast<uint32_t>(width * height * 4), 0},
                      std::move(pixels));
}
} // namespace Formats

} // namespace fvp
