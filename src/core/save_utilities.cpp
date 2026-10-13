#include <bit>
#include <vector>

#include "engine/backlog_entry.hpp"
#include "engine/save_information.hpp"
#include "image/image.hpp"
#include "util/container/fixed_stack.hpp"
#include "util/file/mapped_file.hpp"

// &ptr is the position where we should grab. These are meant to be used to just call in succession when we need to load a new save.
namespace fvp
{

namespace Core
{

// Should be equivalent to _sprintf(buffer, "%s/save/s%03d.bin", save_file_directory, save_number)
// std format will pad the zeroes properly
// std::vformat("{}/save/s{:03}.bin", std::make_format_args(save_file_directory_, save_number))
inline SaveInformation GetSaveInformation(const Utility::MappedFile &save_file, size_t &ptr)
{
  SaveInformation save_info;
  save_info.year = save_file.GetAndIncrement<std::endian::little, uint16_t>(ptr);
  save_info.month = save_file.GetAndIncrement<std::endian::little, uint8_t>(ptr);
  save_info.day = save_file.GetAndIncrement<std::endian::little, uint8_t>(ptr);
  save_info.day_of_week = save_file.GetAndIncrement<std::endian::little, uint8_t>(ptr);
  save_info.hour = save_file.GetAndIncrement<std::endian::little, uint8_t>(ptr);
  save_info.minute = save_file.GetAndIncrement<std::endian::little, uint8_t>(ptr);

  uint16_t size{};
  size = save_file.GetAndIncrement<std::endian::little, uint16_t>(ptr);
  if(size != 0)
  {
    auto data = save_file.Get(ptr, size + 1);
    save_info.byte_stream_1 = std::vector<std::byte>(data.data(), data.size() + data.data() + 1);
    save_info.byte_stream_1.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save_file.GetAndIncrement<std::endian::little, uint16_t>(ptr);
  if(size != 0)
  {
    auto data = save_file.Get(ptr, size + 1);
    save_info.byte_stream_2 = std::vector<std::byte>(data.data(), data.size() + data.data() + 1);
    save_info.byte_stream_2.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save_file.GetAndIncrement<std::endian::little, uint16_t>(ptr);
  if(size != 0)
  {
    auto data = save_file.Get(ptr, size + 1);
    save_info.preview_text = std::vector<std::byte>(data.data(), data.size() + data.data() + 1);
    save_info.preview_text.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  return save_info;
}

inline Image::Image GetSavePreviewImage(const Utility::MappedFile &save_file, uint32_t width, uint32_t height, size_t &ptr)
{
  // We read size data until the image data
  // This goes through all the save metadata
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

inline Utility::FixedStack<BacklogEntry, 100> GetBacklog(const Utility::MappedFile &save_file, size_t &ptr)
{
  constexpr size_t backlog_total_entries{100};
  size_t currently_filled{save_file.GetAndIncrement<std::endian::little, size_t>(ptr)};
  size_t current_head{save_file.GetAndIncrement<std::endian::little, size_t>(ptr)};

  BacklogEntry backlog[backlog_total_entries];

  uint32_t size_holder{};
  for(size_t i{}; i < backlog_total_entries; ++i)
  {
    // Speaker
    size_holder = save_file.GetAndIncrement<std::endian::little, uint32_t>(ptr); 
    if(size_holder != 0)
    {
      backlog[i].speaker = save_file.Get(ptr, size_holder);
    }
    ptr += size_holder; 

    // Spoken Text
    size_holder = save_file.GetAndIncrement<std::endian::little, uint32_t>(ptr); 
    if(size_holder != 0)
    {
      backlog[i].spoken_text = save_file.Get(ptr, size_holder);
    }
    ptr += size_holder;

    // Some ID?
    backlog[i].maybe_text_id = save_file.GetAndIncrement<std::endian::little, uint32_t>(ptr);
  }

  return Utility::FixedStack<BacklogEntry, backlog_total_entries>(std::move(backlog), current_head, currently_filled);
}

}
}
