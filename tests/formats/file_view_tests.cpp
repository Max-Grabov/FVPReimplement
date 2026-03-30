#include "../../src/formats/file_view.hpp"

#include "gtest/gtest.h"

#include <cstdint>
#include <fstream>
#include <string>

TEST(FileViewTest, InvalidPath)
{
  AstralAir::Formats::View view{""};
  EXPECT_FALSE(view.ValidPath());
}

TEST(FileViewTest, ValidPath)
{
  AstralAir::Formats::View view{"./CMakeLists.txt"};
  EXPECT_TRUE(view.ValidPath());
}

TEST(FileViewTest, FileSizeRead)
{
  AstralAir::Formats::View view{"./AstralAirData/voice.bin"};

  EXPECT_TRUE(view.ValidPath());

  // Byte size
  EXPECT_EQ(view.GetFileSize(), 1554116583);
}

TEST(FileViewTest, FileReadUInt8)
{
  AstralAir::Formats::View view{"./AstralAirData/voice.bin"};
  uint64_t test_array[8] = {0b00011000, 0b10000001, 0b00000000, 0b00000000,
                            0b00011011, 0b00001011, 0b00000101, 0b00000000};

  EXPECT_TRUE(view.ValidPath());
  for(uint64_t it{0}; it < 8; ++it)
  {
    EXPECT_EQ(view.Read<uint8_t>(it), test_array[it]);
  }
}

TEST(FileViewTest, FileReadUInt32)
{
  AstralAir::Formats::View view{"./AstralAirData/voice.bin"};
  uint64_t test_array[8] = {0x0008118,  0x00050b1b, 0x00000000, 0x000b1843,
                            0x00009941, 0x0000000a, 0x000bb184, 0x00008ef3};

  EXPECT_TRUE(view.ValidPath());
  for(uint64_t it{0}; it < 8; ++it)
  {
    EXPECT_EQ(view.Read<uint32_t>(it), test_array[it]);
  }
}

TEST(FileViewTest, FileReadUInt64)
{
  AstralAir::Formats::View view{"./AstralAirData/voice.bin"};
  uint64_t test_array[8] = {0x00050b1b00008118, 0x000b184300000000, 0x0000000a00009941,
                            0x00008ef3000bb184, 0x000c407700000014, 0x0000001e0000b368,
                            0x0000c62a000cf3df, 0x000dba0900000028};

  EXPECT_TRUE(view.ValidPath());

  for(uint64_t it{0}; it < 8; ++it)
  {
    EXPECT_EQ(view.Read<uint64_t>(it), test_array[it]);
  }
}

TEST(FileViewTest, FileViewMove)
{
  AstralAir::Formats::View view{"./CMakeLists.txt"};
  EXPECT_TRUE(view.ValidPath());

  AstralAir::Formats::View other(std::move(view));
  EXPECT_TRUE(other.ValidPath());
}

TEST(FileViewTest, FileReadString)
{
  std::ofstream file_to_read("./AstralAirData/test.bin", std::ios::out | std::ios::binary);

  char data[6] = "abcba";
  file_to_read.write(reinterpret_cast<const char *>(data), sizeof(data));

  file_to_read.close();

  AstralAir::Formats::View view{"./AstralAirData/test.bin"};
  EXPECT_TRUE(view.ValidPath());

  // offset 0 size 5
  std::string read_data{view.ReadString(0, 5)};
  for(size_t it{0}; it < 5; ++it)
  {
    EXPECT_EQ(data[it], read_data[it]);
  }
}
