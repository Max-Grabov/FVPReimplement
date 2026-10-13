#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace fvp
{

namespace Core
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
} // namespace Core
} // namespace fvp
