#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace fvp
{

namespace Formats
{

class HcbFormat
{
private:
  using BinaryStream = std::span<const std::byte>;
  struct MetaData
  {
    uint32_t sys_call_start;;
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

  void GetSysCallMap();
};
} // namespace Formats

} // namespace fvp
