#include "audio_stream.hpp"
#include "bin.hpp"

#include "decoder.hpp"

#include "gtest/gtest.h"
#include <SDL3/SDL.h>
#include <optional>

TEST(AudioTest, DecodeTest)
{
  using namespace fvp::Audio;
  using fvp::Formats::BinFormat;
  using fvp::Formats::View;

  BinFormat bin("./AstralAirData/voice.bin");
  bin.OpenAndRead();

  fvp::Formats::View voice_view("./AstralAirData/voice.bin");
  std::vector<std::byte> query =
      voice_view.Read(8 + voice_view.Read<uint32_t>(0) * 12 + voice_view.Read<uint32_t>(8), 9);

  auto result = bin.GetChunk(query);
  std::optional<AudioStream> stream = DecodeOggContainer(result);
  EXPECT_TRUE(stream);
  EXPECT_EQ(stream.value().GetChannels(), 1);
  EXPECT_EQ(stream.value().GetRate(), 48000);
}

TEST(AudioTest, WAVDecodeTest)
{
  using namespace fvp::Audio;
  using fvp::Formats::BinFormat;
  using fvp::Formats::View;

  BinFormat sys_bin("./AstralAirData/se_sys.bin");
  sys_bin.OpenAndRead();
  BinFormat env_bin("./AstralAirData/se_env.bin");
  env_bin.OpenAndRead();
  BinFormat se_bin("./AstralAirData/se.bin");
  se_bin.OpenAndRead();

  fvp::Formats::View se_sys_view("./AstralAirData/se_sys.bin");
  fvp::Formats::View se_env_view("./AstralAirData/se_env.bin");
  fvp::Formats::View se_view("./AstralAirData/se.bin");

  std::vector<std::byte> sys_query =
      se_sys_view.Read(8 + se_sys_view.Read<uint32_t>(0) * 12 + se_sys_view.Read<uint32_t>(8), 3);
  std::vector<std::byte> env_query =
      se_env_view.Read(8 + se_env_view.Read<uint32_t>(0) * 12 + se_env_view.Read<uint32_t>(8), 3);
  std::vector<std::byte> se_query =
      se_view.Read(8 + se_view.Read<uint32_t>(0) * 12 + se_view.Read<uint32_t>(8), 3);

  std::optional<AudioStream> sys_stream = DecodeWAV(sys_bin.GetChunk(sys_query));
  std::optional<AudioStream> env_stream = DecodeWAV(env_bin.GetChunk(env_query));
  std::optional<AudioStream> stream = DecodeWAV(se_bin.GetChunk(se_query));

  EXPECT_TRUE(sys_stream);
  EXPECT_TRUE(env_stream);
  EXPECT_TRUE(stream);
}
