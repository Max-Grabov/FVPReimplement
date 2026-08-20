#pragma once

#include <bitset>
#include <cmath>
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

  // inclusive start, not inclusive end
  template<size_t S>
  static inline void SetBitsFromValue(std::bitset<S> &set, size_t bit_start_index, size_t bit_end_index, uint32_t value, size_t value_bit_offset)
  {
    if(bit_start_index > bit_end_index) return;
    if(bit_start_index >= S) return;
    if((uint64_t{1} << (value_bit_offset + bit_end_index - bit_start_index)) > std::numeric_limits<uint32_t>::max()) return;

    for(size_t i{bit_start_index}; i < bit_end_index; ++i)
    {
      set[i] = (value & (uint32_t{1} << (value_bit_offset + i - bit_start_index))) != 0;
    }
  };

  template<BitSetGettable T, size_t S>
  static inline T GetType(const std::bitset<S> &set, size_t bit_offset)
  {
    if(S < bit_offset + (sizeof(T) * 8))
    {
      throw std::out_of_range("bit_offset + template size is larger than size");
    }
    
    T data{};

    for(size_t i{}; i < sizeof(T) * 8; ++i)
    {
      if(set[i + bit_offset])
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
