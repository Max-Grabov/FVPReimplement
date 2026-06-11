#include "binary_stream_util.hpp"
#include "gtest/gtest.h"

#include <vector>

TEST(UtilTest, TestGet)
{
  using namespace AstralAir::Utility;

  std::vector<std::byte> t{};
  t.emplace_back(std::byte(0));
  t.emplace_back(std::byte(0));
  t.emplace_back(std::byte(0));
  t.emplace_back(std::byte(3));

  uint32_t test_value{Get<uint32_t>(t, 0)};
  EXPECT_EQ(test_value, 0x03000000);
}

TEST(UtilTest, TestConvertEndian)
{
  using namespace AstralAir::Utility;

  std::vector<std::byte> t{};
  t.emplace_back(std::byte(0));
  t.emplace_back(std::byte(0));
  t.emplace_back(std::byte(0));
  t.emplace_back(std::byte(3));

  uint32_t test_value{Get<uint32_t>(t, 0)};
  EXPECT_EQ(test_value, 0x03000000);
  ConvertToEndian<std::endian::little>(test_value);
  EXPECT_EQ(test_value, 0x00000003);
}

TEST(UtilTest, TestConvertEndianType)
{
  using namespace AstralAir::Utility;

  std::vector<uint32_t> t = {0x01010101, 0x20003000, 0x12345678}; 
 
  for(uint32_t i{}; i < t.size(); ++i)
  {
    ConvertToEndian<std::endian::little, uint32_t>(t[i]);
  }
  
  EXPECT_EQ(t[0], 0x01010101);
  EXPECT_EQ(t[1], 0x00300020);
  EXPECT_EQ(t[2], 0x78563412);
};
