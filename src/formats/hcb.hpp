#pragma once

#include "file_view.hpp"

namespace AstralAir
{

namespace Formats
{

class HcbFormat
{
private:
  struct MetaData
  {
    uint64_t script_length;
    uint64_t script_start;
  };
  MetaData meta_data_;
  View file_view_;

public:
  explicit HcbFormat(const std::string &path);
  ~HcbFormat() = default;
  HcbFormat(const HcbFormat &other) = delete;
  HcbFormat(HcbFormat &&other) = default;
  HcbFormat &operator=(const HcbFormat &other) = delete;
  HcbFormat &operator=(HcbFormat &&other) = default; 

  void OpenAndRead();
};
} // namespace Formats

} // namespace AstralAir
