#pragma once

#include "util/file_view.hpp"

#include <cstdint>
#include <map>
#include <vector>

namespace fvp
{

namespace Formats
{

class BinFormat
{
private:
  using EntryName = std::vector<std::byte>;

  struct Entry
  {
    uint32_t offset;
    uint32_t size;
  };

  std::map<EntryName, Entry> data_collection_;
  Utility::View file_view_;

public:
  explicit BinFormat(const std::string &path);
  ~BinFormat() = default;
  BinFormat(const BinFormat &other) = delete;
  BinFormat(BinFormat &&other) = default;
  BinFormat &operator=(const BinFormat &other) = delete;
  BinFormat &operator=(BinFormat &&other) = default;

  size_t CollectionSize() const;
  void OpenAndRead();
  bool HasEntry(const EntryName &) const;
  Entry QueryEntry(const EntryName &) const;
  std::vector<std::byte> GetChunk(const EntryName &, const uint32_t = 0);
};

} // namespace Formats

} // namespace fvp
