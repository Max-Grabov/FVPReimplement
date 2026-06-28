#include "bin.hpp"
#include "image.hpp"
#include "image_decoder.hpp"
#include "gtest/gtest.h"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

TEST(StreamTest, TestGetImage)
{
  using namespace fvp::Image;
  using fvp::Formats::BinFormat;
  using fvp::Formats::View;

  BinFormat vis_bin("./AstralAirData/graph_vis.bin");
  vis_bin.OpenAndRead();
  BinFormat sd_bin("./AstralAirData/graph_sd.bin");
  sd_bin.OpenAndRead();
  BinFormat bg_bin("./AstralAirData/graph_bg.bin");
  bg_bin.OpenAndRead();
  BinFormat bs_bin("./AstralAirData/graph_bs.bin");
  bs_bin.OpenAndRead();
  BinFormat g_bin("./AstralAirData/graph.bin");
  g_bin.OpenAndRead();

  View vis_view("./AstralAirData/graph_vis.bin");
  View sd_view("./AstralAirData/graph_sd.bin");
  View bg_view("./AstralAirData/graph_bg.bin");
  View bs_view("./AstralAirData/graph_bs.bin");
  View g_view("./AstralAirData/graph.bin");

  std::vector<std::byte> vis_query =
      vis_view.Read(8 + vis_view.Read<uint32_t>(0) * 12 + vis_view.Read<uint32_t>(8), 9);
  std::vector<std::byte> sd_query =
      sd_view.Read(8 + sd_view.Read<uint32_t>(0) * 12 + sd_view.Read<uint32_t>(8), 7);
  std::vector<std::byte> bg_query =
      bg_view.Read(8 + bg_view.Read<uint32_t>(0) * 12 + bg_view.Read<uint32_t>(8), 9);
  std::vector<std::byte> bs_query =
      bs_view.Read(8 + bs_view.Read<uint32_t>(0) * 12 + bs_view.Read<uint32_t>(8), 17);
  std::vector<std::byte> g_query =
      g_view.Read(8 + g_view.Read<uint32_t>(0) * 12 + g_view.Read<uint32_t>(8), 8);

  std::vector<std::byte> vis_data = vis_bin.GetChunk(vis_query);
  std::vector<std::byte> sd_data = sd_bin.GetChunk(sd_query);
  std::vector<std::byte> bg_data = bg_bin.GetChunk(bg_query);
  std::vector<std::byte> bs_data = bs_bin.GetChunk(bs_query);
  std::vector<std::byte> g_data = g_bin.GetChunk(g_query);

  std::optional<Image> vis_hzc = CreateImage(std::move(vis_data));
  std::optional<Image> sd_hzc = CreateImage(std::move(sd_data));
  std::optional<Image> bg_hzc = CreateImage(std::move(bg_data));
  std::optional<Image> bs_hzc = CreateImage(std::move(bs_data));
  std::optional<Image> g_hzc = CreateImage(std::move(g_data));

  std::vector<std::byte> foo_v = {std::byte{0}};
  std::optional<Image> foo = CreateImage(std::move(foo_v));

  EXPECT_TRUE(vis_hzc);
  EXPECT_TRUE(sd_hzc);
  EXPECT_TRUE(bg_hzc);
  EXPECT_TRUE(bs_hzc);
  EXPECT_TRUE(g_hzc);
  EXPECT_FALSE(foo);
}
