
#include "bin.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>

// File decoding for the binary files is heavily based on
// https://github.com/morkt/GARbro/blob/master/ArcFormats/Favorite/ArcBIN.cs
// As these goats figured out the way to decode the binary files.
namespace AstralAir
{

namespace Formats
{

BinFormat::BinFormat(const std::string &path) : file_view_(path) {}

size_t BinFormat::CollectionSize() const { return data_collection_.size(); }

void BinFormat::OpenAndRead()
{
  uint32_t count{file_view_.Read<uint32_t>(0)};
  uint32_t index_size{count * 12};
  uint32_t name_index_size{file_view_.Read<uint32_t>(4)};
  uint32_t file_offset{8};
  uint32_t filename_offset{file_view_.Read<uint32_t>(file_offset)};
  uint64_t names_base_position{file_offset + index_size};

  // tspmo
  std::vector<std::byte> total_name_index_buffer =
      file_view_.Read(names_base_position + filename_offset, name_index_size - filename_offset);

  uint32_t holder{};
  for(int i = 0; i < count; ++i)
  {
    filename_offset = file_view_.Read<uint32_t>(file_offset);

    if(filename_offset >= name_index_size)
      throw std::runtime_error("Offset bigger than name index");

    // For some reason, the initial reading for file offset makes the read to the name_buffer have a
    // massive size instead of the proper size. We instead opt for putting the buffer entirely into
    // the vector to hold all of the data, and then copying over the specific name sizes,this lead
    // to a massive time speedup (10x) for reading data. Most likely there is a faster way to do
    // this via memory mapping but not too familiar yet. A key limitation is for some reason one of
    // the data points for "voice" doesnt follow the same formatting as the rest (read isnt
    // producing monotonic increasing values due to this). So we can't assume that other files
    // follow a monotonic increasing offset pattern. Otherwise, we could use previous and next
    // offsets to directly read into the name_buffer vector Instead, we opt for reading until null
    // terminator for each name in the big buffer and then pass that into the individual name
    // buffers
    uint32_t beginning{holder};
    while(total_name_index_buffer[holder++] != std::byte(0))
      ;

    EntryName name_buffer(holder - beginning - 1);
    std::copy(total_name_index_buffer.begin() + beginning,
              total_name_index_buffer.begin() + holder - 1, name_buffer.begin());

    Entry entry{file_view_.Read<uint32_t>(file_offset + 4),
                file_view_.Read<uint32_t>(file_offset + 8)};
    data_collection_.emplace(std::move(name_buffer), std::move(entry));
    file_offset += 12;
  }
}

/* Returns whether an entry offset and size exist for a query. If the binary file hasn't been
 * OpenAndRead() yet, this will obviously return false as the map is empty */
bool BinFormat::HasEntry(const EntryName &name) const { return data_collection_.contains(name); }

BinFormat::Entry BinFormat::QueryEntry(const EntryName &name) const
{
  return data_collection_.at(name);
}

/* This function grabs a certain chunk of data from a query. The default chunk size is 0, which
 * grabs the full block of memory of the data and returns it back. Supplying a positive amount for
 * the chunk returns a portion of it, being the amount bytes. This allows for grabbing pieces of
 * asset data (for audio streams especially) instead of being forced to load 100MB into RAM at once
 * If supplying a size that is greater than the size of the Entry, it will cap out to the entry
 * size.
 */
std::vector<std::byte> BinFormat::GetChunk(const EntryName &name, const uint32_t amount)
{
  Entry entry_data = QueryEntry(name);
  return file_view_.Read(entry_data.offset,
                         ((amount <= 0 || amount > entry_data.size) ? entry_data.size : amount));
}

} // namespace Formats
} // namespace AstralAir
