#include <vector>

#include "engine/backlog_entry.hpp"
#include "engine/save_information.hpp"
#include "image/image.hpp"
#include "util/container/fixed_stack.hpp"
#include "util/file/mapped_file.hpp"

namespace fvp
{

namespace Core
{

// Should be equivalent to _sprintf(buffer, "%s/save/s%03d.bin", save_file_directory, save_number)
// std format will pad the zeroes properly
// std::vformat("{}/save/s{:03}.bin", std::make_format_args(save_file_directory_, save_number))
inline SaveInformation GetSaveInformation(const Utility::MappedFile &save_file, size_t starting_byte)
{
  SaveInformation save_info;
  size_t ptr{starting_byte};

  save_info.year = save_file.Get<std::endian::little, uint16_t>(ptr);
  ptr += 2;
  save_info.month = save_file.Get<std::endian::little, uint8_t>(ptr++);
  save_info.day = save_file.Get<std::endian::little, uint8_t>(ptr++);
  save_info.day_of_week = save_file.Get<std::endian::little, uint8_t>(ptr++);
  save_info.hour = save_file.Get<std::endian::little, uint8_t>(ptr++);
  save_info.minute = save_file.Get<std::endian::little, uint8_t>(ptr++);

  uint16_t size{};
  size = save_file.Get<std::endian::little, uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    auto data = save_file.Get(ptr, size + 1);
    save_info.byte_stream_1 = std::vector<std::byte>(data.data(), data.size() + data.data() + 1);
    save_info.byte_stream_1.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save_file.Get<std::endian::little, uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    auto data = save_file.Get(ptr, size + 1);
    save_info.byte_stream_2 = std::vector<std::byte>(data.data(), data.size() + data.data() + 1);
    save_info.byte_stream_2.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save_file.Get<std::endian::little, uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    auto data = save_file.Get(ptr, size + 1);
    save_info.preview_text = std::vector<std::byte>(data.data(), data.size() + data.data() + 1);
    save_info.preview_text.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  return save_info;
}

inline Image::Image GetSavePreviewImage(const Utility::MappedFile &save_file, uint32_t width, uint32_t height, size_t starting_byte)
{
  // We read size data until the image data
  // This goes through all the save metadata
  size_t ptr{starting_byte};
  ptr += (save_file.Get<std::endian::little, uint16_t>(ptr) + 2);
  ptr += (save_file.Get<std::endian::little, uint16_t>(ptr) + 2);
  ptr += (save_file.Get<std::endian::little, uint16_t>(ptr) + 2);

  // Image is raw bytes, so we can just fread and create
  auto pixels = save_file.Get(ptr, width * height * 4);

  // This is safe as far as I know (narrowing the type). Image metadata as defined in zlib for full
  // screen images fits in uint16_t
  return Image::Image({0, static_cast<uint16_t>(width), static_cast<uint16_t>(height), 0, 0, 0,
                       static_cast<uint32_t>(width * height * 4), 0},
                      std::vector<std::byte>(pixels.data(), pixels.data() + pixels.size()));
}

inline Utility::FixedStack<BacklogEntry, 100> GetBacklog(const Utility::MappedFile &save_file, size_t starting_byte)
{
  
}

}
}
