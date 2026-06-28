#include "hcb.hpp"
#include "gtest/gtest.h"

TEST(HCBTest, TestOpenAndRead)
{
  using fvp::Formats::HcbFormat;

  HcbFormat hcb("./AstralAirData/Snow.hcb");
  //hcb.OpenAndRead(); 
}
