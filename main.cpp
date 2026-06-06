#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio_playback.hpp"
#include "audio_stream.hpp"
#include "file_view.hpp"
#include "bin.hpp"
#include "decoder.hpp"
#include "hzc_stream.hpp"
#include <vector>

#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> 

static SDL_AudioStream *bgm_stream = NULL;
static SDL_Window *window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture_image = NULL;
static AstralAir::Audio::AudioStream g_bgm_data_stream;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])

{  
  window = SDL_CreateWindow("Test", 1920, 1080, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  renderer = SDL_CreateRenderer(window, NULL);

  AstralAir::Formats::BinFormat bgm_bin("./AstralAirData/bgm.bin"); 
  AstralAir::Formats::BinFormat voice_bin("./AstralAirData/voice.bin");
  AstralAir::Formats::BinFormat bg_bin("./AstralAirData/graph_vis.bin");
   
  bgm_bin.OpenAndRead();
  voice_bin.OpenAndRead(); 
  bg_bin.OpenAndRead();

  // For testing, we will read the bgm 0 name from the file so it is easy to query, essentially copying the first part of BinFormat::OpenAndRead() for sample
  AstralAir::Formats::View bgm_view("./AstralAirData/bgm.bin");  
  std::vector<std::byte> query = bgm_view.Read(728, 3);

  auto bgm_data_stream = AstralAir::Audio::DecodeOggContainer(bgm_bin.GetChunk(query));

  if (!SDL_Init(SDL_INIT_AUDIO)) {
      SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
      return SDL_APP_FAILURE;
  }

  bgm_stream = AstralAir::Audio::CreateAudioStream(bgm_data_stream);
  g_bgm_data_stream = bgm_data_stream;

  if (!bgm_stream) {
      SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
      return SDL_APP_FAILURE;
  }

  // Image Testing
  AstralAir::Formats::View bg_view("./AstralAirData/graph_vis.bin");
  std::vector<std::byte> image_query =
      bg_view.Read(8 + bg_view.Read<uint32_t>(0) * 12 + bg_view.Read<uint32_t>(8), 9);

  std::vector<std::byte> data = bg_bin.GetChunk(image_query);
  std::optional<AstralAir::Image::HZC_Stream> hzc = AstralAir::Image::HZC_Stream::Construct(std::move(data));
  if(hzc.has_value())
  {
    // Needs to be refactored into free function for texture creation
    texture_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, 1280, 2048);
    SDL_UpdateTexture(texture_image, nullptr, reinterpret_cast<const void *>(hzc.value().GetImage().get()), 3 * 1280);
  } 
  SDL_ResumeAudioStreamDevice(bgm_stream);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; 
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{ 
  static bool played{false};

  if(!played)
  {
    SDL_RenderClear(renderer); 
    SDL_RenderTexture(renderer, texture_image, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    AstralAir::Audio::PlayAudio(bgm_stream, g_bgm_data_stream);
    played = true;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{ 
  SDL_DestroyTexture(texture_image);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

