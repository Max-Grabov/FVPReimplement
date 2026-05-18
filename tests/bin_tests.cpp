#include "bin.hpp"
#include "data.hpp"

#include "gtest/gtest.h"

#include <array>

TEST(BinTest, TestOpen)
{
  using AstralAir::Data::AstralAirData;
  using AstralAir::Formats::BinFormat;

  BinFormat bin("./AstralAirData/voice.bin");

  std::vector<AstralAirData> data{bin.OpenAndRead()};
  std::array<char, 9> comparison = {'0', '1', '0', '0', '0', '0', '0', '0', '0'};

  EXPECT_EQ(33048, data.size());

  for(size_t it{0}; it < 9; ++it)
  {
    EXPECT_EQ(static_cast<char>(data[0].GetBuffer()[it]), comparison[it]);
  }
}

TEST(BinTest, TestPrint)
{
  using AstralAir::Data::AstralAirData;
  using AstralAir::Formats::BinFormat;

  BinFormat bin("./AstralAirData/voice.bin");

  std::vector<AstralAirData> data{bin.OpenAndRead()};
  long index{};
  for(const auto &d : data)
  {
    std::cout << index++ << "\n";
    std::cout << d.GetBuffer().size() << "\n";
  }
}
