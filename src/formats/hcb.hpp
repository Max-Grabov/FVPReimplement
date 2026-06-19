#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <span>

namespace AstralAir
{

namespace Formats
{

class HcbFormat
{
private:
  using BinaryStream = std::span<const std::byte>;
  struct MetaData
  {
    uint64_t script_length;
    uint64_t script_start;
  };

  MetaData meta_data_;
  BinaryStream data_; 
  bool is_mmaped_;

public:
  explicit HcbFormat(const std::string &);
  ~HcbFormat();
  HcbFormat(const HcbFormat &) = delete;
  HcbFormat(HcbFormat &&) = delete;
  HcbFormat &operator=(const HcbFormat &) = delete;
  HcbFormat &operator=(HcbFormat &&) = delete;

  void OpenAndRead();
};
} // namespace Formats

} // namespace AstralAir
