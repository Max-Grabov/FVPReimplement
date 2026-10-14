#include "render_window.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace fvp
{

namespace Core
{

RenderWindow::RenderWindow(std::string_view window_name, int width_resolution,
                           int height_resolution, int window_options)
{
  window_ =
    SDL_CreateWindow(window_name.data(), width_resolution, height_resolution, window_options);

  if(!window_)
  {
    throw std::runtime_error(
      "Error creating SDL Window with name: " + std::string(window_name.data()) +
      ", and resolution " + std::to_string(width_resolution) + "x" +
      std::to_string(height_resolution));
  }

  renderer_ = SDL_CreateRenderer(window_, NULL);

  if(!renderer_)
  {
    throw std::runtime_error("Error creating SDL renderer with associated window!");
  }
}

RenderWindow::~RenderWindow()
{
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

RenderWindow::RenderWindow(RenderWindow &&other) noexcept
    : window_(std::exchange(other.window_, {})), renderer_(std::exchange(other.renderer_, {}))
{
}

RenderWindow &RenderWindow::operator=(RenderWindow &&other) noexcept
{
  window_ = other.window_;
  renderer_ = other.renderer_;

  other.window_ = nullptr;
  other.renderer_ = nullptr;

  return *this;
}

void RenderWindow::RendererPresent() { SDL_RenderPresent(renderer_); }

// TODO RAII
SDL_Texture *RenderWindow::CreateTexture(SDL_PixelFormat pixel_format,
                                         SDL_TextureAccess texture_access_policy, int texture_width,
                                         int texture_height)
{
  return SDL_CreateTexture(renderer_, pixel_format, texture_access_policy, texture_width,
                           texture_height);
}

bool RenderWindow::RenderTexture(SDL_Texture *texture, SDL_FRect *source_rectangle_region,
                                 SDL_FRect *destination_rectangle_region)
{
  return SDL_RenderTexture(renderer_, texture, source_rectangle_region,
                           destination_rectangle_region);
}

} // namespace Core
} // namespace fvp
