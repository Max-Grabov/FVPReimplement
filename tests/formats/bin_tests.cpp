#include "../../src/data/data.hpp"
#include "../../src/formats/bin.hpp"

#include "gtest/gtest.h"

#include <iostream>

TEST(BinTest, TestOpen)
{
  using AstralAir::Data::AstralAirData;
  using AstralAir::Formats::BinFormat;

  BinFormat bin("./AstralAirData/voice.bin");

  std::vector<AstralAirData> data{bin.OpenAndRead()};

  std::cout << data[0].GetData() << "\n";
}
