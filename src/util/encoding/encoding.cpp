
#include <bitset>
#include <cstddef>
#include <cstring>
#include <vector>
#include <span>
#include <cstdint>
#include "encoding.hpp"
#include "shift_jis_table.hpp"

namespace fvp
{

namespace Utility
{

std::vector<std::byte> ConvertShiftJISToUTF8String(std::span<const std::byte> stream)
{
  // Most 2 byte shift jis values get turned into 3 bytes in UTF8 (e.g. 0xE382A2 for ア in UTF8 and 0x8341 in full width shift JIS)
  // Therefore we multiply by 2 to prevent any vector resizing, and then shrink to fit at the end.

  std::vector<std::byte> utf8_string;
  utf8_string.reserve(stream.size() * 2);

  for(size_t i{}; i < stream.size(); ++i)
  {
    // Standard ASCII 7 bit -> 1 byte length
    if(static_cast<uint8_t>(stream.data()[i]) >= 0x0 && static_cast<uint8_t>(stream.data()[i]) <= 0x7f)
    {
      // TODO
    }

    // 2 byte length for everything else
    else
    {      
      // Prevents endianness needing to be checked for systems.
      uint32_t code_point{GetUTFCodePointFromShiftJISValue(static_cast<uint16_t>((reinterpret_cast<const uint8_t*>(stream.data())[i] << 8) + reinterpret_cast<const uint8_t*>(stream.data())[i + 1]))};
      std::vector<std::byte> converted = GetUTFHexRepresentationFromCodePoint(code_point); 

      for(const auto b : converted)
      {
        utf8_string.emplace_back(b);
      }

      // Increment 1 more for the double width input
      i++;
    }
  }

  utf8_string.shrink_to_fit();
  return utf8_string;
}

uint32_t GetUTFCodePointFromShiftJISValue(uint16_t shift_jis_value)
{
  static constexpr uint16_t ROW_LOWER_BOUND{0x81};
  static constexpr uint16_t ROW_UPPER_BOUND{0xFC};
  static constexpr uint16_t COLUMN_LOWER_BOUND{0x40};
  static constexpr uint16_t COLUMN_UPPER_BOUND{0xFC};
  if(((shift_jis_value >> 8) & 0xFF) < ROW_LOWER_BOUND || ((shift_jis_value >> 8) & 0xFF) > ROW_UPPER_BOUND)
  {
    return INVALID_LOOKUP;
  }

  // In the column of the lookup table, the 0x7F column is removed causing it to skip by 1 in index, therefore 0x7f column values are invalid lookups
  if((shift_jis_value & 0xFF) < COLUMN_LOWER_BOUND || (shift_jis_value & 0xFF) > COLUMN_UPPER_BOUND || (shift_jis_value & 0xFF) == 0x7F)
  {
    return INVALID_LOOKUP;
  }

  // also due to the 0x7f row not being included, we have to subtract an extra 1 if our index is greater than 0x7f so we can lookup properly
  return SHIFT_JIS_TO_UNICODE_LUT[((shift_jis_value >> 8) & 0xFF) - ROW_LOWER_BOUND][(shift_jis_value & 0xFF) - COLUMN_LOWER_BOUND - ((shift_jis_value & 0xFF) > 0x7F ? 1 : 0)];
}

// TODO this can cause many heap allocations, find a better solution?
std::vector<std::byte> GetUTFHexRepresentationFromCodePoint(uint32_t code_point)
{ 
  if(code_point >= 0x0 && code_point <= 0x007f)
  {
    std::vector<std::byte> hex_representation(1);
    std::bitset<8> set;

    set[7] = false;

    SetBitsFromValue<8>(set, 0, 7, code_point, 0);

    hex_representation[0] = static_cast<std::byte>(GetType<uint8_t, 8>(set, 0)); 
    return hex_representation;
  }

  else if(code_point >= 0x0080 && code_point <= 0x07ff)
  {
    std::vector<std::byte> hex_representation(2);
    std::bitset<16> set;

    set[15] = true;
    set[14] = true;
    set[13] = false;

    set[7] = true;
    set[6] = false;

    SetBitsFromValue<16>(set, 0, 6, code_point, 0);
    SetBitsFromValue<16>(set, 8, 13, code_point, 6);
    
    hex_representation[0] = static_cast<std::byte>(GetType<uint8_t, 16>(set, 8));
    hex_representation[1] = static_cast<std::byte>(GetType<uint8_t, 16>(set, 0));
    return hex_representation;
  }
  
  else if(code_point >= 0x0800 && code_point <= 0xffff)
  {
    std::vector<std::byte> hex_representation(3);
    std::bitset<24> set;

    set[23] = true;
    set[22] = true;
    set[21] = true;
    set[20] = false;

    set[15] = true;
    set[14] = false;

    set[7] = true;
    set[6] = false;

    SetBitsFromValue<24>(set, 0, 6, code_point, 0);
    SetBitsFromValue<24>(set, 8, 14, code_point, 6);
    SetBitsFromValue<24>(set, 16, 20, code_point, 12);
  
    hex_representation[0] = static_cast<std::byte>(GetType<uint8_t, 24>(set, 16));
    hex_representation[1] = static_cast<std::byte>(GetType<uint8_t, 24>(set, 8));
    hex_representation[2] = static_cast<std::byte>(GetType<uint8_t, 24>(set, 0));
    return hex_representation;
  }

  else if(code_point >= 0x10000 && code_point <= 0x10ffff)
  {
    std::vector<std::byte> hex_representation(4);
    std::bitset<32> set;

    set[31] = true;
    set[30] = true;
    set[29] = true;
    set[28] = true;
    set[27] = false;

    set[23] = true;
    set[22] = false;

    set[15] = true;
    set[14] = false;

    set[7] = true;
    set[6] = false;

    SetBitsFromValue<32>(set, 0, 6, code_point, 0);
    SetBitsFromValue<32>(set, 8, 14, code_point, 6);
    SetBitsFromValue<32>(set, 16, 22, code_point, 12);
    SetBitsFromValue<32>(set, 24, 27, code_point, 18);
  
    hex_representation[0] = static_cast<std::byte>(GetType<uint8_t, 32>(set, 24));
    hex_representation[1] = static_cast<std::byte>(GetType<uint8_t, 32>(set, 16));
    hex_representation[2] = static_cast<std::byte>(GetType<uint8_t, 32>(set, 8));
    hex_representation[3] = static_cast<std::byte>(GetType<uint8_t, 32>(set, 0));
    return hex_representation;
  }

  // Invalid range
  return {};
}

}
}
