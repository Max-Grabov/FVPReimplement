#include "formats/persistent_file.hpp"

#include "gtest/gtest.h"
#include <cstdint>
#include <fcntl.h>
#include <system_error>

TEST(FileTest, TestRead)
{
  fvp::Formats::PersistentFile file("./AstralAirData/Snow.hcb");
  EXPECT_TRUE(file.Valid());

  EXPECT_EQ(file.Get<uint32_t>(0), 0x0058d8b4);
}

TEST(FileTest, TestInvalidWrite)
{
  fvp::Formats::PersistentFile file("./AstralAirData/test.bin");
  EXPECT_TRUE(file.Valid());
  EXPECT_THROW(file.Write(10, static_cast<uint8_t>(0x1010)), std::system_error);
}

TEST(FileTest, TestWrite)
{
  fvp::Formats::PersistentFile file("./AstralAirData/test.bin", O_RDWR);
  EXPECT_TRUE(file.Valid());
  file.Write(10, 0x61);
  EXPECT_EQ(file.Get<uint8_t>(10), 0x61);
}
