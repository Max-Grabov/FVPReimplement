#include "util/binary_stream_util.hpp"
#include "file_view.hpp"

#include "gtest/gtest.h"

#include <cstdint>
#include <fstream>

TEST(FileViewTest, InvalidPath)
{
  fvp::Formats::View view{""};
  EXPECT_FALSE(view.ValidPath());
}

TEST(FileViewTest, ValidPath)
{
  fvp::Formats::View view{"./CMakeLists.txt"};
  EXPECT_TRUE(view.ValidPath());
}

TEST(FileViewTest, FileSizeRead)
{
  fvp::Formats::View view{"./AstralAirData/voice.bin"};

  EXPECT_TRUE(view.ValidPath());

  // Byte size
  EXPECT_EQ(view.GetFileSize(), 1554116583);
}

TEST(FileViewTest, FileReadUInt8)
{
  fvp::Formats::View view{"./AstralAirData/voice.bin"};
  uint8_t test_array[8] = {0b00011000, 0b10000001, 0b00000000, 0b00000000,
                           0b00011011, 0b00001011, 0b00000101, 0b00000000};

  EXPECT_TRUE(view.ValidPath());
  for(uint8_t it{0}; it < 8; ++it)
  {
    EXPECT_EQ(view.Read<uint8_t>(it), test_array[it]);
  }
}

TEST(FileViewTest, FileReadUInt32)
{
  fvp::Formats::View view{"./AstralAirData/voice.bin"};
  /*0x18810000  0x1b0b0500*/

  uint32_t test_array[8] = {0x18810000, 0x1b0b0500, 0x00000000, 0x43180b00,
                            0x41990000, 0x0a000000, 0x84b10b00, 0xf38e0000};

  EXPECT_TRUE(view.ValidPath());
  for(uint32_t it{0}; it < 8; ++it)
  {
    fvp::Utility::ConvertToEndian<std::endian::little>(test_array[it]);
    EXPECT_EQ(view.Read<uint32_t>(it * sizeof(uint32_t)), test_array[it]);
  }
}

TEST(FileViewTest, FileReadUInt64)
{
  fvp::Formats::View view{"./AstralAirData/voice.bin"};
  uint64_t test_array[8] = {0x00050b1b00008118, 0x000b184300000000, 0x0000000a00009941,
                            0x00008ef3000bb184, 0x000c407700000014, 0x0000001e0000b368,
                            0x0000c62a000cf3df, 0x000dba0900000028};

  EXPECT_TRUE(view.ValidPath());

  for(uint64_t it{0}; it < 8; ++it)
  {
    EXPECT_EQ(view.Read<uint64_t>(it * sizeof(uint64_t)), test_array[it]);
  }
}

TEST(FileViewTest, FileViewMove)
{
  fvp::Formats::View view{"./CMakeLists.txt"};
  EXPECT_TRUE(view.ValidPath());

  fvp::Formats::View other(std::move(view));
  EXPECT_TRUE(other.ValidPath());
}

TEST(FileViewTest, FileReadString)
{
  std::ofstream file_to_read("./AstralAirData/test.bin", std::ios::out | std::ios::binary);

  char data[6] = "abcba";
  file_to_read.write(reinterpret_cast<const char *>(data), sizeof(data));

  file_to_read.close();

  fvp::Formats::View view{"./AstralAirData/test.bin"};
  EXPECT_TRUE(view.ValidPath());

  // offset 0 size 5
  auto read_data{view.Read(0, 5)};
  for(size_t it{}; it < 5; ++it)
  {
    EXPECT_EQ(data[it], static_cast<char>(read_data[it]));
  }
}
