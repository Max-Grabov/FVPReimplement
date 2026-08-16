#pragma once

#include <cstddef>
#include <vector>
#include <span>
#include <cstdint>

namespace fvp
{

namespace Util
{

namespace ShiftJISConstants
{
  static inline constexpr uint16_t HIRAGANA_CODE_POINT_START_HEX{0x3041};
  static inline constexpr uint16_t HIRAGANA_SHIFT_JIS_START_HEX{0x829F};
  static inline constexpr uint16_t HIRAGANA_SHIFT_JIS_END_HEX{0x82F1};
  static inline constexpr uint16_t KATAKANA_CODE_POINT_START_HEX{0x30A1}; 
  static inline constexpr uint16_t KATAKANA_SHIFT_JIS_START_HEX{0x8340};
  static inline constexpr uint16_t KATAKANA_SHIFT_JIS_END_HEX{0x8396};
}

std::vector<std::byte> ConvertShiftJISToUTF8String(std::span<std::byte> stream);

uint32_t GetUTFCodePointFromShiftJISValue(uint16_t shift_jis_value);

}
}
