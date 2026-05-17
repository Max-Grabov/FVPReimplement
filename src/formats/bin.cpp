
#include "bin.hpp"
#include "data.hpp" 

#include <cstdint>
#include <filesystem>

// File decoding for the binary files is heavily based on
// https://github.com/morkt/GARbro/blob/master/ArcFormats/Favorite/ArcBIN.cs
// As these goats figured out the way to decode the binary files.
namespace AstralAir
{

using Data::AstralAirData;

namespace Formats
{

BinFormat::BinFormat(const std::string &path) : file_view_(path)
{ 
  // We assume the files are exactly the proper data files, there should be proper checks before
  // reading more
  count_ = file_view_.Read<uint32_t>(0);
  index_size_ = (count_ * 12 / sizeof(uint32_t));

  // Offset is the size of 1 of the templated data type
  data_name_size_ = file_view_.Read<uint32_t>(1);

  if(std::filesystem::path(path).filename() == "voice")
  {
    file_type_ = FileType::VOICE;
  }
  else if(std::filesystem::path(path).filename() == "bgm")
  {
    file_type_ = FileType::BGM;
  }
  // TODO MORE
}

std::vector<AstralAirData> BinFormat::OpenAndRead()
{
  std::vector<AstralAirData> data_collection;
  uint32_t file_offset{2};
  uint64_t int_32_names_base{2 + index_size_};
  for(int i = 0; i < count_; ++i)
  {
    uint32_t filename_offset{file_view_.Read<uint32_t>(file_offset)}; 
    if(filename_offset >= data_name_size_)
      return {};

    // ReadStringBuffer takes offsets in single bytes, so we multiply by uint32_t size to convert back
    std::vector<std::byte> buffer{file_view_.ReadStringBuffer(
        sizeof(uint32_t) * (int_32_names_base + filename_offset), data_name_size_ - filename_offset)};   
    uint32_t offset{file_view_.Read<uint32_t>(file_offset + 1)};
    uint32_t data{file_view_.Read<uint32_t>(file_offset + 2)};
    data_collection.emplace_back(std::move(buffer), offset, data);
    file_offset += 3;
  }

  return data_collection;
}

} // namespace Formats
} // namespace AstralAir
