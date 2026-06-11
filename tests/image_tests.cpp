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

  BinFormat vis_bin("./AstralAirData/graph_vis.bin");
  vis_bin.OpenAndRead();
  BinFormat sd_bin("./AstralAirData/graph_sd.bin"); 
  sd_bin.OpenAndRead(); 
  BinFormat bg_bin("./AstralAirData/graph_bg.bin");
  bg_bin.OpenAndRead();
  BinFormat bs_bin("./AstralAirData/graph_bs.bin");
  bs_bin.OpenAndRead();
  
  View vis_view("./AstralAirData/graph_vis.bin");
  View sd_view("./AstralAirData/graph_sd.bin");
  View bg_view("./AstralAirData/graph_bg.bin");
  View bs_view("./AstralAirData/graph_bs.bin");

  std::vector<std::byte> vis_query =
      vis_view.Read(8 + vis_view.Read<uint32_t>(0) * 12 + vis_view.Read<uint32_t>(8), 9);
  std::vector<std::byte> sd_query =
      sd_view.Read(8 + sd_view.Read<uint32_t>(0) * 12 + sd_view.Read<uint32_t>(8), 7);
  std::vector<std::byte> bg_query =
      bg_view.Read(8 + bg_view.Read<uint32_t>(0) * 12 + bg_view.Read<uint32_t>(8), 9);
  std::vector<std::byte> bs_query =
      bs_view.Read(8 + bs_view.Read<uint32_t>(0) * 12 + bs_view.Read<uint32_t>(8), 17);
  
  std::vector<std::byte> vis_data = vis_bin.GetChunk(vis_query);
  std::vector<std::byte> sd_data = sd_bin.GetChunk(sd_query);
  std::vector<std::byte> bg_data = bg_bin.GetChunk(bg_query);
  std::vector<std::byte> bs_data = bs_bin.GetChunk(bs_query);

  std::optional<HZC_Stream> vis_hzc = HZC_Stream::Construct(std::move(vis_data)); 
  std::optional<HZC_Stream> sd_hzc = HZC_Stream::Construct(std::move(sd_data));
  std::optional<HZC_Stream> bg_hzc = HZC_Stream::Construct(std::move(bg_data));
  std::optional<HZC_Stream> bs_hzc = HZC_Stream::Construct(std::move(bs_data));

  EXPECT_TRUE(vis_hzc);
  EXPECT_TRUE(sd_hzc);
  EXPECT_TRUE(bg_hzc);
  EXPECT_TRUE(bs_hzc);
}
