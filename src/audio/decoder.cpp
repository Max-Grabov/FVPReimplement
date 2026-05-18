#include "decoder.hpp"
#include "vorbis/codec.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ogg/ogg.h>

namespace AstralAir
{

namespace Audio
{

void DecodeOggContainer(const std::vector<std::byte> &input_buffer)
{
  ogg_sync_state sync_state;
  ogg_stream_state stream_state;
  ogg_page ogg_page;
  ogg_packet ogg_packet;
  vorbis_info v_info;
  vorbis_comment v_comment;
  vorbis_dsp_state v_dsp;
  vorbis_block v_block;
  bool stream_end{false};

  ogg_sync_init(&sync_state);

  while(1)
  {
    if(input_buffer.size() == 0)
    {
      std::cerr << "Input stream for decoding is empty\n";

      // Return an empty whatever once return type is decided
      return;
    }

    long byte_size = input_buffer.size();

    // Add one for null terminator when moving the input_buffer's data to the buffer
    char *buffer = ogg_sync_buffer(&sync_state, byte_size);

    if(!buffer)
    {
      std::cerr << "Input buffer for decoding is empty\n";
      return;
    }

    std::memcpy(buffer, input_buffer.data(), byte_size);

    ogg_sync_wrote(&sync_state, byte_size);

    if(ogg_sync_pageout(&sync_state, &ogg_page) != 1)
      break;

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

      return;
    }
    if(ogg_stream_packetout(&stream_state, &ogg_packet) != 1)
    {
      std::cerr << "Error reading initial packet of OggStream\n";
      return;
    }
    if(vorbis_synthesis_headerin(&v_info, &v_comment, &ogg_packet) < 0)
    {
      std::cerr << "OggStream has no vorbis data\n";
      return;
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
          return;
        }
        result = vorbis_synthesis_headerin(&v_info, &v_comment, &ogg_packet);
        if(result < 0)
        {
          std::cerr << "Corrupted\n";
          return;
        }
        ++count;
      }
    }

    char **pin = v_comment.user_comments;
    while(*pin)
      std::cout << *pin << "\n";
    ++pin;

    std::cout << "channel " << v_info.channels << " rate " << v_info.rate << "\n";
  }
}

} // namespace Audio
} // namespace AstralAir
