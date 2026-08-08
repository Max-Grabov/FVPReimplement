#include "util/persistent_file.hpp"

#include "gtest/gtest.h"
#include <cstdint>
#include <fcntl.h>

using namespace fvp::Utility;

TEST(FileTest, TestRead)
{
  auto file = PersistentFile("./AstralAirData/Snow.hcb", PersistentFile::Permissions::READ,
                             PersistentFile::CreateFile::NO_CREATE_FILE);
  EXPECT_EQ(file.Get<uint32_t>(0), 0x0058d8b4);
}

TEST(FileTest, TestInvalidWrite)
{
  auto file = PersistentFile("./AstralAirData/test.bin", PersistentFile::Permissions::READ,
                             PersistentFile::CreateFile::NO_CREATE_FILE);
  EXPECT_THROW(file.Write(10, static_cast<uint8_t>(0x1010)),
               PersistentFile::no_write_allowed_exception);
}

TEST(FileTest, TestNoCreateThrow)
{
  EXPECT_THROW(PersistentFile("foobarfoobar", PersistentFile::Permissions::READ,
                              PersistentFile::CreateFile::NO_CREATE_FILE),
               PersistentFile::create_file_exception);
}

TEST(FileTest, TestWrite)
{
  auto file = PersistentFile("./AstralAirData/test.bin", PersistentFile::Permissions::READ_WRITE,
                             PersistentFile::CreateFile::NO_CREATE_FILE);
  file.Write(10, 0x61);
  EXPECT_EQ(file.Get<uint8_t>(10), 0x61);
}
