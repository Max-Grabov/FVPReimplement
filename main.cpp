#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio_playback.hpp"
#include "audio_stream.hpp"
#include "bin.hpp"
#include "decoder.hpp"
#include "file_view.hpp"
#include "image_decoder.hpp"
#include "image.hpp"
#include <vector>

#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_AudioStream *bgm_stream = NULL;
static SDL_AudioStream *se_stream = NULL;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture_image = NULL;
static SDL_Texture *texture_bg = NULL;
static AstralAir::Audio::AudioStream g_bgm_data_stream;
static AstralAir::Audio::AudioStream g_se_data_stream;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])

{
  window =
      SDL_CreateWindow("Test", 1920, 1080, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  renderer = SDL_CreateRenderer(window, NULL);

  AstralAir::Formats::BinFormat bgm_bin("./AstralAirData/bgm.bin");
  AstralAir::Formats::BinFormat bg_vis_bin("./AstralAirData/graph_vis.bin");
  AstralAir::Formats::BinFormat bg_bin("./AstralAirData/graph_bg.bin");
  AstralAir::Formats::BinFormat sys_bin("./AstralAirData/se_sys.bin");
 
  bgm_bin.OpenAndRead();
  bg_vis_bin.OpenAndRead();
  bg_bin.OpenAndRead();
  sys_bin.OpenAndRead();

  // For testing, we will read the bgm 0 name from the file so it is easy to query, essentially
  // copying the first part of BinFormat::OpenAndRead() for sample
  AstralAir::Formats::View bg_view("./AstralAirData/graph_vis.bin");
  AstralAir::Formats::View bg_bg_view("./AstralAirData/graph_bg.bin");
  AstralAir::Formats::View bgm_view("./AstralAirData/bgm.bin");
  AstralAir::Formats::View se_sys_view("./AstralAirData/se_sys.bin");

  std::vector<std::byte> query = bgm_view.Read(728, 3);
  std::vector<std::byte> image_query =
      bg_view.Read(8 + bg_view.Read<uint32_t>(0) * 12 + bg_view.Read<uint32_t>(8), 9);
  std::vector<std::byte> bg_query =
      bg_bg_view.Read(8 + bg_bg_view.Read<uint32_t>(0) * 12 + bg_bg_view.Read<uint32_t>(8), 9);
  std::vector<std::byte> sys_query =
      se_sys_view.Read(8 + se_sys_view.Read<uint32_t>(0) * 12 + se_sys_view.Read<uint32_t>(8), 3);
  

  std::optional<AstralAir::Audio::AudioStream> bgm_data_stream = AstralAir::Audio::DecodeOggContainer(bgm_bin.GetChunk(query));
  std::optional<AstralAir::Audio::AudioStream> sys_stream = AstralAir::Audio::DecodeWAV(sys_bin.GetChunk(sys_query));
  
  if(!SDL_Init(SDL_INIT_AUDIO))
  {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  
  bgm_stream = AstralAir::Audio::CreateAudioStream(bgm_data_stream.value());
  g_bgm_data_stream = bgm_data_stream.value();
  se_stream = AstralAir::Audio::CreateAudioStream(sys_stream.value());
  g_se_data_stream = sys_stream.value();

  if(!bgm_stream)
  {
    SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  std::vector<std::byte> image_data = bg_vis_bin.GetChunk(image_query);
  std::vector<std::byte> bg_data = bg_bin.GetChunk(bg_query);

  // Test out textures for BG
  
  std::optional<AstralAir::Image::Image> image = AstralAir::Image::CreateImage(std::move(image_data));
  if(image.has_value())
  {

    texture_image = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC,
                                      image.value().GetMetaData().width, image.value().GetMetaData().height);
    SDL_UpdateTexture(texture_image, nullptr,
                      reinterpret_cast<const void *>(image.value().GetPixels().data()),
                      3 * image.value().GetMetaData().width);
  }

  
  std::optional<AstralAir::Image::Image> bg_image = AstralAir::Image::CreateImage(std::move(bg_data));
  if(bg_image.has_value())
  {
    texture_bg = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC,
                                   bg_image.value().GetMetaData().width, bg_image.value().GetMetaData().height);
    SDL_UpdateTexture(texture_bg, nullptr, reinterpret_cast<const void *>(bg_image.value().GetPixels().data()),
                      3 * bg_image.value().GetMetaData().width);
  }

  SDL_ResumeAudioStreamDevice(bgm_stream);
  SDL_ResumeAudioStreamDevice(se_stream);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  if(event->type == SDL_EVENT_QUIT)
  {
    return SDL_APP_SUCCESS;
  }
  else if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
  {
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture_image, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
  static bool played{false};

  if(!played)
  {
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture_bg, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    AstralAir::Audio::PlayAudio(se_stream, g_se_data_stream);
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
