
#include <cstddef>
#include <vector>
#include <span>
#include <cstdint>
#include "encoding.hpp"

namespace fvp
{

namespace Util
{

std::vector<std::byte> ConvertShiftJISToUTF8String(std::span<std::byte> stream)
{
  return {};
}

uint32_t GetUTFCodePointFromShiftJISValue(uint16_t shift_jis_value)
{
  if(shift_jis_value >= ShiftJISConstants::HIRAGANA_SHIFT_JIS_START_HEX && 
     shift_jis_value <= ShiftJISConstants::HIRAGANA_SHIFT_JIS_END_HEX)
  {
    return (shift_jis_value - ShiftJISConstants::HIRAGANA_SHIFT_JIS_START_HEX) + 
      ShiftJISConstants::HIRAGANA_SHIFT_JIS_START_HEX;
  }

  if(shift_jis_value >= ShiftJISConstants::KATAKANA_SHIFT_JIS_START_HEX && 
     shift_jis_value <= ShiftJISConstants::KATAKANA_SHIFT_JIS_END_HEX)
  {
    return (shift_jis_value - ShiftJISConstants::KATAKANA_SHIFT_JIS_START_HEX) + 
      ShiftJISConstants::KATAKANA_SHIFT_JIS_START_HEX;
  }

  return {};
}

}
}
