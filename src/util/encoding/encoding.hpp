#pragma once

#include <bitset>
#include <iostream>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>
#include <span>
#include <cstdint>

namespace fvp
{

namespace Utility
{

namespace
{
  template <typename T>
  concept BitSetGettable = std::is_integral_v<T> || std::is_floating_point_v<T>;

  static inline constexpr uint16_t HIRAGANA_CODE_POINT_START_HEX{0x3041};
  static inline constexpr uint16_t HIRAGANA_SHIFT_JIS_START_HEX{0x829F};
  static inline constexpr uint16_t HIRAGANA_SHIFT_JIS_END_HEX{0x82F1};
  static inline constexpr uint16_t KATAKANA_CODE_POINT_START_HEX{0x30A1}; 
  static inline constexpr uint16_t KATAKANA_SHIFT_JIS_START_HEX{0x8340};
  static inline constexpr uint16_t KATAKANA_SHIFT_JIS_END_HEX{0x8396};

  // inclusive start, not inclusive end
  template<size_t S>
  static inline auto set_bits = [](std::bitset<S> &set, size_t start_index, size_t end_index, uint32_t code_point, size_t code_point_offset)
  {
    if(start_index > end_index) return;
    if(start_index >= S) return;
    if((uint64_t{1} << (code_point_offset + end_index - start_index)) > std::numeric_limits<uint32_t>::max()) return;

    for(size_t i{start_index}; i < end_index; ++i)
    {
      set[i] = (code_point & (uint32_t{1} << (code_point_offset + i - start_index))) != 0;
    }
  };

  template<BitSetGettable T, size_t S>
  T GetType(const std::bitset<S> &set, size_t offset)
  {
    if(S < offset + (sizeof(T) * 8))
    {
      throw std::out_of_range("Offset + template size is larger than size");
    }
    
    T data{};

    for(size_t i{}; i < sizeof(T) * 8; ++i)
    {
      if(set[i + offset])
      {
        data |= 1 << i;
      }
    }

    return data;
  }
}

std::vector<std::byte> ConvertShiftJISToUTF8String(std::span<std::byte> stream);

uint32_t GetUTFCodePointFromShiftJISValue(uint16_t shift_jis_value);

std::vector<std::byte> GetUTFHexRepresentationFromCodePoint(uint32_t code_point);

}
}
