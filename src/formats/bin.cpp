
#include "bin.hpp"

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <utility>

// File decoding for the binary files is heavily based on
// https://github.com/morkt/GARbro/blob/master/ArcFormats/Favorite/ArcBIN.cs
// As these goats figured out the way to decode the binary files.
namespace AstralAir
{

namespace Formats
{

BinFormat::BinFormat(const std::string &path) : file_view_(path)
{
  // We assume the files are exactly the proper data files, there should be proper checks before
  // reading more 
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

size_t BinFormat::CollectionSize() const { return data_collection_.size(); }
  
void BinFormat::OpenAndRead()
{
  uint32_t count = file_view_.Read<uint32_t>(0);
  uint32_t index_size = count * 12;
  uint32_t name_index_size = file_view_.Read<uint32_t>(4);
  uint32_t file_offset{8};
  uint64_t names_base_position{file_offset + index_size};

  for(int i = 0; i < count; ++i)
  {
    uint32_t filename_offset{file_view_.Read<uint32_t>(file_offset)};

    if(filename_offset >= name_index_size)
      throw std::runtime_error("Offset bigger than name index");

    EntryName name_buffer{
        file_view_.Read(names_base_position + filename_offset, name_index_size - filename_offset)};
    Entry entry{file_view_.Read<uint32_t>(file_offset + 4), file_view_.Read<uint32_t>(file_offset + 8)};
    data_collection_.emplace(std::move(name_buffer), std::move(entry));
    file_offset += 12;
  }
}

/* Returns whether an entry offset and size exist for a query. If the binary file hasn't been OpenAndRead() yet, this will obviously return false as the map is empty */
bool BinFormat::HasEntry(const EntryName &name) const { return data_collection_.find(name) != data_collection_.end(); }

BinFormat::Entry BinFormat::QueryEntry(const EntryName &name) const { return data_collection_.at(name); }

/* This function grabs a certainc chunk of data from a query. The default chunk size is 0, which grabs the full block of memory of the data and returns it back. Supplying an amount for the chunk returns a portion of it, being a size of amount bytes. This allows for grabbing pieces of asset data (for audio streams) instead of being forced to load 100MB into RAM at once
 */
std::vector<std::byte> BinFormat::GetChunk(const EntryName &name, const uint32_t amount)
{
  Entry entry_data = QueryEntry(name);
  return file_view_.Read(entry_data.offset, (amount <= 0 ? entry_data.size : amount));
}

} // namespace Formats
} // namespace AstralAir
