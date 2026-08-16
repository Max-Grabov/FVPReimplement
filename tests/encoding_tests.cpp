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
