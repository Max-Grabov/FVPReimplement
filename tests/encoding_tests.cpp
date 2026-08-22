#include "util/encoding/encoding.hpp"
#include <cstdint>

#include "gtest/gtest.h"
#include <string_view>

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

TEST(EncodingTest, Test2ByteShiftJISToCodePoint)
{
  // 片仮名
  for(size_t i{0x8340}; i < 0x837e; ++i)
  {
    EXPECT_EQ(GetUTFCodePointFrom2ByteShiftJISValue(i), 0x30a1 + i - 0x8340);
  }

  // 片仮名 pt2, due to the table col. at 0x7f not existing, so there is a small jump
  for(size_t i{0x8380}; i < 0x8397; ++i)
  {
    EXPECT_EQ(GetUTFCodePointFrom2ByteShiftJISValue(i), 0x30e0 + i - 0x8380);
  }

  // 平仮名
  for(size_t i{0x829f}; i < 0x82f1; ++i)
  {
    EXPECT_EQ(GetUTFCodePointFrom2ByteShiftJISValue(i), 0x3041 + i - 0x829f);
  }

  uint16_t 靴{0x8c43};
  EXPECT_EQ(GetUTFCodePointFrom2ByteShiftJISValue(靴), 0x9774);

  // 拭、植、殖、燭、織、職、色、触、食、蝕
  uint32_t res[10] = {0x62ed, 0x690d, 0x6b96, 0x71ed, 0x7e54,
                      0x8077, 0x8272, 0x89e6, 0x98df, 0x8755};
  for(size_t i{0x9040}; i < 0x904a; ++i)
  {
    EXPECT_EQ(GetUTFCodePointFrom2ByteShiftJISValue(i), res[i - 0x9040]);
  }

  // 紂 紜 紕 紊 絅 絋 紮 紲 紿 紵
  uint32_t res_2[10] = {0x7d02, 0x7d1c, 0x7d15, 0x7d0a, 0x7d45,
                        0x7d4b, 0x7d2e, 0x7d32, 0x7d3f, 0x7d35};
  for(size_t i{0xe340}; i < 0xe34a; ++i)
  {
    EXPECT_EQ(GetUTFCodePointFrom2ByteShiftJISValue(i), res_2[i - 0xe340]);
  }
}

TEST(EncodingTest, Test1ByteShiftJISToCodePoint)
{
  for(uint8_t i{0xa1}; i < 0xdf; ++i)
  {
    EXPECT_EQ(GetUTFCodePointFrom1ByteShiftJISValue(i), 0xff61 + i - 0xa1);
  }
}

TEST(EncodingTest, TestKanjiShiftJISToUTFBytes)
{
  // 拭、植、殖、燭、織、職、色、触、食、蝕
  std::string_view shift_jis_sequence =
      "\x90\x40\x90\x41\x90\x42\x90\x43\x90\x44\x90\x45\x90\x46\x90\x47\x90\x48\x90\x49";

  auto result = ConvertShiftJISToUTF8String(std::span<const std::byte>(
      reinterpret_cast<const std::byte *>(shift_jis_sequence.data()), shift_jis_sequence.size()));

  std::string_view expected_utf8_result =
      "\xE6\x8B\xAD\xE6\xA4\x8D\xE6\xAE\x96\xE7\x87\xAD\xE7\xB9\x94\xE8\x81\xB7\xE8\x89\xB2\xE8\xA7"
      "\xA6\xE9\xA3\x9F\xE8\x9D\x95";

  EXPECT_EQ(result.size(), expected_utf8_result.size());

  for(size_t i{}; i < result.size(); ++i)
  {
    EXPECT_EQ(static_cast<char>(result[i]), expected_utf8_result[i]);
  }

  std::string_view sentence = "\x92\x4E\x82\xC9\x82\xC5\x82\xE0\x96\xB2\x82\xF0\x8C\xA9\x82\xE9\x8C"
                              "\xA0\x97\x98\x82\xCD\x82\xA0\x82\xE9\x81\x42\x83\x6E\x83\x41\x83\x41"
                              "\x83\x40\xB1\x20\x20\xB1\x20\x20\xB1\x20\x20\xB1";

  auto sentence_result = ConvertShiftJISToUTF8String(std::span<const std::byte>(
      reinterpret_cast<const std::byte *>(sentence.data()), sentence.size()));

  std::string_view expected_sentence_utf8_result =
      "\xE8\xAA\xB0\xE3\x81\xAB\xE3\x81\xA7\xE3\x82\x82\xE5\xA4\xA2\xE3\x82\x92\xE8\xA6\x8B\xE3\x82"
      "\x8B\xE6\xA8\xA9\xE5\x88\xA9\xE3\x81\xAF\xE3\x81\x82\xE3\x82\x8B\xE3\x80\x82\xE3\x83\x8F\xE3"
      "\x82\xA2\xE3\x82\xA2\xE3\x82\xA1\xEF\xBD\xB1\x20\x20\xEF\xBD\xB1\x20\x20\xEF\xBD\xB1\x20\x20"
      "\xEF\xBD\xB1";

  EXPECT_EQ(sentence_result.size(), expected_sentence_utf8_result.size());

  for(size_t i{}; i < expected_sentence_utf8_result.size(); ++i)
  {
    EXPECT_EQ(static_cast<char>(sentence_result[i]), expected_sentence_utf8_result[i]);
  }
}
