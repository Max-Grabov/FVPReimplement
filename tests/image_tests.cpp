#include "bin.hpp"
#include "hzc_stream.hpp"
#include "gtest/gtest.h"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

TEST(HZC_StreamTest, TestConstruct)
{
  using namespace AstralAir::Image;
  using AstralAir::Formats::BinFormat;
  using AstralAir::Formats::View;

  BinFormat bin("./AstralAirData/graph_vis.bin");
  bin.OpenAndRead();

  View g_view("./AstralAirData/graph_vis.bin");
  std::vector<std::byte> query =
      g_view.Read(8 + g_view.Read<uint32_t>(0) * 12 + g_view.Read<uint32_t>(8), 9);

  std::vector<std::byte> data = bin.GetChunk(query);
  EXPECT_TRUE(HZC_Stream::Construct(std::move(data)));

  std::vector<std::byte> junk = g_view.Read(2, 9);
  EXPECT_FALSE(HZC_Stream::Construct(std::move(junk)));
}

TEST(HZC_StreamTest, TestGetImage)
{
  using namespace AstralAir::Image;
  using AstralAir::Formats::BinFormat;
  using AstralAir::Formats::View;

  BinFormat bin("./AstralAirData/graph_vis.bin");
  bin.OpenAndRead();

  View g_view("./AstralAirData/graph_vis.bin");
  std::vector<std::byte> query =
      g_view.Read(8 + g_view.Read<uint32_t>(0) * 12 + g_view.Read<uint32_t>(8), 9);

  std::vector<std::byte> data = bin.GetChunk(query);
  std::optional<HZC_Stream> hzc = HZC_Stream::Construct(std::move(data));
  if(hzc.has_value())
  {
    hzc.value().PrintImageMetaData();
  }
}
