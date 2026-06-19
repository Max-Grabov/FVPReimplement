#include "decoder.hpp"
#include "audio_stream.hpp"
#include "binary_stream_util.hpp"
#include <bit>

#include "vorbis/codec.h"
#include <ogg/config_types.h>
#include <ogg/ogg.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>

// Ogg Stream decoding adapted from this example here ->
// https://github.com/xiph/vorbis/blob/main/examples/decoder_example.c
namespace AstralAir
{

namespace Audio
{

/* Returns an AudioStream if the stream successfully passes the checks necessary to be a WAV format,
 * otherwise returns a null optional. This function will invalidate the input_buffer as it will be
 * moved to the new audio stream.
 */
std::optional<AudioStream> DecodeWAV(std::vector<std::byte> &&input_buffer)
{
  // RIFF Chunk
  if(input_buffer.size() < 44)
  {
    return std::nullopt;
  }

  uint32_t id{Utility::ConvertToEndian<std::endian::little>(
                Utility::Get<uint32_t>(input_buffer, 0))};
  if(id != 0x52494646)
  {
    return std::nullopt;
  }

  uint32_t size{Utility::Get<uint32_t>(input_buffer, 4)};
  if(size + 8 != input_buffer.size())
  {
    return std::nullopt;
  }

  uint32_t format{Utility::ConvertToEndian<std::endian::little>(
                    Utility::Get<uint32_t>(input_buffer, 8))};
  if(format != 0x57415645)
  {
    return std::nullopt;
  }

  // fmt sub chunk
  uint32_t fmt_format{Utility::ConvertToEndian<std::endian::little>(Utility::Get<uint32_t>(input_buffer, 12))};
  if(fmt_format != 0x666d7420)
  {
    return std::nullopt;
  }

  uint32_t fmt_chunk_size{Utility::Get<uint32_t>(input_buffer, 16)};
  if(fmt_chunk_size != 0x00000010)
  {
    return std::nullopt;
  }

  uint16_t channels{Utility::Get<uint16_t>(input_buffer, 22)};
  uint16_t rate{Utility::Get<uint16_t>(input_buffer, 24)};
  uint16_t bits_per_sample{Utility::Get<uint16_t>(input_buffer, 34)};
  uint32_t byte_rate{Utility::Get<uint32_t>(input_buffer, 28)};
  if(byte_rate != rate * channels * bits_per_sample / 8)
  {
    return std::nullopt;
  }

  uint16_t block_align{Utility::Get<uint16_t>(input_buffer, 32)};
  if(block_align != channels * bits_per_sample / 8)
  {
    return std::nullopt;
  }

  // data sub chunk
  uint32_t data_id{Utility::ConvertToEndian<std::endian::little>(Utility::Get<uint32_t>(input_buffer, 36))};
  if(data_id != 0x4C495354)
  {
    return std::nullopt;
  }

  uint32_t pad_string{Utility::ConvertToEndian<std::endian::little>(Utility::Get<uint32_t>(input_buffer, 110))};
  if(pad_string != 0x50414420)
  {
    return std::nullopt;
  }

  uint32_t pad_value{Utility::Get<uint32_t>(input_buffer, 114)};
  uint32_t data_string{Utility::ConvertToEndian<std::endian::little>(Utility::Get<uint32_t>(input_buffer, 118 + pad_value))};
  if(data_string != 0x64617461)
  {
    return std::nullopt;
  }

  // This size is in bytes, so for floats we divide by 4 when putting into the buffer
  uint32_t data_size{Utility::Get<uint32_t>(input_buffer, 118 + pad_value + 4)};
  if(data_size % 4 != 0)
  {
    return std::nullopt;
  }

  std::vector<float> pcm(data_size / 4);

  std::memcpy(pcm.data(), input_buffer.data() + 118 + pad_value + 4 + 4,
              data_size / 4);
 
  return AudioStream(std::move(pcm), channels, rate);
}

std::optional<AudioStream> DecodeOggContainer(const std::vector<std::byte> &input_buffer)
{
  ogg_sync_state sync_state;
  ogg_stream_state stream_state;
  ogg_page ogg_page;
  ogg_packet ogg_packet;
  vorbis_info v_info;
  vorbis_comment v_comment;
  vorbis_dsp_state v_dsp;
  vorbis_block v_block;

  ogg_sync_init(&sync_state);

  if(input_buffer.size() == 0)
  {
    std::cerr << "Input stream for decoding is empty\n";
    return std::nullopt;
  }

  long byte_size = input_buffer.size();

  // Add one for null terminator when moving the input_buffer's data to the buffer
  char *buffer = ogg_sync_buffer(&sync_state, byte_size);

  if(!buffer)
  {
    std::cerr << "Input buffer for decoding is empty\n";
    return std::nullopt;
  }

  std::memcpy(buffer, input_buffer.data(), byte_size);

  ogg_sync_wrote(&sync_state, byte_size);

  if(ogg_sync_pageout(&sync_state, &ogg_page) != 1)
  {
    std::cerr << "Error with sync pageout\n";
    return std::nullopt;
  }

  ogg_stream_init(&stream_state, ogg_page_serialno(&ogg_page));
  vorbis_info_init(&v_info);
  vorbis_comment_init(&v_comment);

  // Standard check to ensure it is vorbis
  if(ogg_stream_pagein(&stream_state, &ogg_page) < 0)
  {
    std::cerr << "Error reading first page of OggStream\n";
    std::cout << ogg_page_version(&ogg_page) << "\n";
    std::cout << ogg_page_serialno(&ogg_page) << "\n";
    std::cout << stream_state.serialno << "\n";

    return std::nullopt;
  }
  if(ogg_stream_packetout(&stream_state, &ogg_packet) != 1)
  {
    std::cerr << "Error reading initial packet of OggStream\n";
    return std::nullopt;
  }
  if(vorbis_synthesis_headerin(&v_info, &v_comment, &ogg_packet) < 0)
  {
    std::cerr << "OggStream has no vorbis data\n";
    return std::nullopt;
  }

  // First two packets
  for(uint8_t count{}; count < 2;)
  {
    if(!ogg_sync_pageout(&sync_state, &ogg_page))
    {
      std::cout << "empty\n";
      break;
    }

    ogg_stream_pagein(&stream_state, &ogg_page);
    while(count < 2)
    {
      auto result = ogg_stream_packetout(&stream_state, &ogg_packet);
      if(!result)
      {
        std::cout << "empty\n";
        break;
      }
      if(result < 0)
      {
        std::cerr << "Corrupted\n";
        return std::nullopt;
      }
      result = vorbis_synthesis_headerin(&v_info, &v_comment, &ogg_packet);
      if(result < 0)
      {
        std::cerr << "Corrupted\n";
        return std::nullopt;
      }
      ++count;
    }
  }

  int conv_size = byte_size / v_info.channels;

  // From the example, we should have parsed the three headers. Now we can initialize the vorbis
  // packet
  std::vector<float> pcm_output;
  long channels{v_info.channels};
  long rate{v_info.rate};

  if(vorbis_synthesis_init(&v_dsp, &v_info) == 0)
  {
    vorbis_block_init(&v_dsp, &v_block);

    // Now we should be able to decode until end of stream
    bool end_of_stream{false};
    while(!end_of_stream)
    {
      auto result = ogg_sync_pageout(&sync_state, &ogg_page);
      if(!result)
        break;
      if(result < 0)
      {
        std::cerr << "Corrupted\n";
        return std::nullopt;
      }
      else
      {
        ogg_stream_pagein(&stream_state, &ogg_page);
        while(1)
        {
          auto second_result = ogg_stream_packetout(&stream_state, &ogg_packet);
          if(!second_result)
            break;
          if(second_result < 0)
          {
            std::cerr << "Corrupted\n";
            return std::nullopt;
          }
          else
          {
            float **pcm;
            int samples{};

            if(vorbis_synthesis(&v_block, &ogg_packet) == 0)
              vorbis_synthesis_blockin(&v_dsp, &v_block);

            while((samples = vorbis_synthesis_pcmout(&v_dsp, &pcm)) > 0)
            {
              bool clipflag{false};
              int bout = std::min(samples, conv_size);
              pcm_output.reserve(bout * v_info.channels);

              for(int j{}; j < bout; j++)
              {
                for(int i{}; i < v_info.channels; i++)
                {
                  pcm_output.push_back(pcm[i][j]);
                }
              }

              if(clipflag)
                std::cerr << "Clipping at frame " << v_dsp.sequence << "\n";

              vorbis_synthesis_read(&v_dsp, bout);
            }
          }
        }
        if(ogg_page_eos(&ogg_page))
          end_of_stream = 1;
      }
    }
    // Stream should be over
    vorbis_block_clear(&v_block);
    vorbis_dsp_clear(&v_dsp);
  }
  else
  {
    std::cerr << "Corrupt Header\n";
  }

  ogg_stream_clear(&stream_state);
  vorbis_comment_clear(&v_comment);
  vorbis_info_clear(&v_info);

  ogg_sync_clear(&sync_state);

  return AudioStream(std::move(pcm_output), channels, rate);
}

} // namespace Audio
} // namespace AstralAir
