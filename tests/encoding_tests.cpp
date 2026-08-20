#include "util/encoding/encoding.hpp"
#include <cstdint>

#include "gtest/gtest.h"

using namespace fvp::Utility;

TEST(EncodingTest, TestCodePointToHex)
{
  uint32_t code_point{0x3042};
  auto result = GetUTFHexRepresentationFromCodePoint(code_point);
  EXPECT_EQ(result[0], static_cast<std::byte>(0xe3));
  EXPECT_EQ(result[1], static_cast<std::byte>(0x81));
  EXPECT_EQ(result[2], static_cast<std::byte>(0x82));

  uint32_t code_point_2{0x3043};
  auto result_2 = GetUTFHexRepresentationFromCodePoint(code_point_2);
  EXPECT_EQ(result_2[0], static_cast<std::byte>(0xe3));
  EXPECT_EQ(result_2[1], static_cast<std::byte>(0x81));
  EXPECT_EQ(result_2[2], static_cast<std::byte>(0x83));
}

TEST(EncodingTest, TestShiftJISKatakanaToCodePoint)
{
  uint16_t ア{0x8341};
  EXPECT_EQ(GetUTFCodePointFromShiftJISValue(ア), 0x30a2);

  uint16_t 靴{0x8c43};
  EXPECT_EQ(GetUTFCodePointFromShiftJISValue(靴), 0x9774);

  // 拭、植、殖、燭、織、職、色、触、食、蝕
  uint32_t res[10] = {0x62ed, 0x690d, 0x6b96, 0x71ed, 0x7e54, 0x8077, 0x8272, 0x89e6, 0x98df, 0x8755};
  for(size_t i{0x9040}; i < 0x904a; ++i)
  {
    EXPECT_EQ(GetUTFCodePointFromShiftJISValue(i), res[i - 0x9040]);  
  }
}
