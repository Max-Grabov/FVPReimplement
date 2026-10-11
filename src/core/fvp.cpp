#include "fvp.hpp"

#include "audio/audio_stream.hpp"
#include "audio/decoder.hpp"
#include "audio_playback.hpp"
#include "engine/resolution_table.hpp"
#include "engine/syscall_entry.hpp"
#include "formats/bin.hpp"
#include "image/image.hpp"
#include "image/image_decoder.hpp"
#include "render_window.hpp"
#include "screen_mode.hpp"
#include "util/encoding/encoding.hpp"
#include "util/file/mapped_file.hpp"

#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

#include <cstdint>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>

namespace fvp
{

namespace Core
{

// TODO IMPLEMENT FIND FILE, THIS IS TEMPORARY FOR TESTING!
FVP::FVP() : save_file_directory_("./AstralAirData"), data_directory_("./AstralAirData")
{
  InitializeData();
}

void FVP::Run()
{
  // TODO Obviously all audio needs to be abstracted
  // Dummy Testing brought from old main.cpp
  fvp::Formats::BinFormat graph_vis_bin("./AstralAirData/graph_vis.bin");
  fvp::Formats::BinFormat bgm_bin("./AstralAirData/bgm.bin");

  graph_vis_bin.OpenAndRead();
  bgm_bin.OpenAndRead();

  fvp::Utility::View graph_vis_view("./AstralAirData/graph_vis.bin");
  fvp::Utility::View bgm_view("./AstralAirData/bgm.bin");

  std::vector<std::byte> query = bgm_view.Read(728, 3);
  std::vector<std::byte> image_query =
      graph_vis_view.Read(8 + graph_vis_view.Read<uint32_t, std::endian::big>(0) * 12 +
                              graph_vis_view.Read<uint32_t, std::endian::big>(8),
                          9);

  std::vector<std::byte> image_data = graph_vis_bin.GetChunk(image_query);
  std::optional<fvp::Image::Image> image = fvp::Image::CreateImage(std::move(image_data));
  SDL_Texture *image_texture =
      rendering_window_->CreateTexture(SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC,
                                       image->GetMetaData().width, image->GetMetaData().height);

  // TODO Put in texture RAII Wrapper
  // (BROUGHT FROM MAIN CPP) Save images are different pixel formats, alpha is just 0xFF, saving
  // this for future reference TODO
  //  texture_save = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRX32, SDL_TEXTUREACCESS_STATIC,
  //                                      save_preview.GetMetaData().width,
  //                                      save_preview.GetMetaData().height);
  //  SDL_UpdateTexture(texture_save, nullptr,
  //                    reinterpret_cast<const void *>(save_preview.GetPixels().data()),
  //                    4 * save_preview.GetMetaData().width);

  SDL_UpdateTexture(image_texture, nullptr,
                    reinterpret_cast<const void *>(image->GetPixels().data()),
                    3 * image->GetMetaData().width);

  SDL_Init(SDL_INIT_AUDIO);

  std::optional<fvp::Audio::AudioStream> bgm_data_stream =
      fvp::Audio::DecodeOggContainer(bgm_bin.GetChunk(query));
  SDL_AudioStream *bgm_stream = fvp::Core::CreateAudioStream(bgm_data_stream.value());
  SDL_ResumeAudioStreamDevice(bgm_stream);
  fvp::Core::PlayAudio(bgm_stream, bgm_data_stream.value());

  SDL_FRect dest_rect{0, 0, static_cast<float>(window_width_), static_cast<float>(window_height_)};
  rendering_window_->RenderTexture(image_texture, nullptr, &dest_rect);
  rendering_window_->RendererPresent();
  SDL_ShowCursor();

  SDL_Event event{};

  // TESTING
  while(1)
  {
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_EVENT_QUIT)
      {
        return;
      }
    }
  }
}

void FVP::InitializeData()
{
  if(!SDL_Init(SDL_INIT_VIDEO))
  {
    throw std::runtime_error(SDL_GetError());
  }

  OpenHCBFile();
  OpenOverallSave();
  OpenWindowAndCursor();
}

void FVP::OpenOverallSave()
{
  try
  {
    overall_save_file_ = std::make_unique<Utility::MappedFile>(
        std::vformat("{}/save/save.bin", std::make_format_args(save_file_directory_)),
        Utility::MappedFile::Permissions::READ_WRITE,
        Utility::MappedFile::CreateFile::NO_CREATE_FILE);

    uint32_t ptr{};

    // Something 8 Bytes, I thought it was Opcode but Im going to hold off on it until I know for
    // sure. memcpy(opcodes_.data() + opcode_count_, overall_save_file_->Data().data(),
    // opcodes_processed_);
    // ptr += opcodes_processed_ * sizeof(Opcode);

    ptr += opcodes_processed_ * 8;
    uint8_t screen_mode_byte{overall_save_file_->GetAndIncrement<std::endian::big, uint8_t>(ptr)};

    // sets this mode based off if the mode is not 0 (windowed), so we have the mapping
    // windowed -> windowed
    // fullscreen -> fullscreen
    // IDK -> fullscreen
    screen_mode_ = static_cast<ScreenMode>(
        (static_cast<ScreenMode>(screen_mode_byte) != ScreenMode::WINDOWED));

    // Similar to above, sets the boolean if not equal to 0. Since we only have to options I just
    // read directly.
    visible_ =
        static_cast<bool>(overall_save_file_->GetAndIncrement<std::endian::big, uint8_t>(ptr));

    uint32_t left_position{overall_save_file_->GetAndIncrement<std::endian::big, uint32_t>(ptr)};
    uint32_t top_position{overall_save_file_->GetAndIncrement<std::endian::big, uint32_t>(ptr)};

    // TODO Handle 2nd window stuff and right position + bottom

    uint8_t cursor_choice{overall_save_file_->GetAndIncrement<std::endian::big, uint8_t>(ptr)};

    // Set our cursor
    if(cursor_choice < 4)
    {
      // Some field must not be false TODO
      if(!false)
      {

        // SDL Cursor selection from the cursor array
        // then if another field is not false, set the cursor TODO
        if(!false)
        {
          current_cursor_ = &cursors_[cursor_choice];
        }
      }

      cursor_choice_ = cursor_choice;
    }

    uint8_t field_0x81{overall_save_file_->GetAndIncrement<std::endian::big, uint8_t>(ptr)};

    // Save preview image dimensions!!!
    preview_save_image_width_ =
        overall_save_file_->GetAndIncrement<std::endian::big, uint32_t>(ptr);
    preview_save_image_height_ =
        overall_save_file_->GetAndIncrement<std::endian::big, uint32_t>(ptr);

    // some 0x100000 length field, it SHOULD be guarranteed to be 0x100000
    constexpr uint32_t unknown_field_0xfdc_length{0x100000};
    std::vector<std::byte> unknown_field_0xfdc(unknown_field_0xfdc_length);
    memcpy(unknown_field_0xfdc.data(),
           overall_save_file_->Get(ptr, unknown_field_0xfdc_length).data(),
           unknown_field_0xfdc_length);
    ptr += unknown_field_0xfdc_length;
    // Some other array 0x40 elements of size 4 bytes

    constexpr uint32_t unknown_field_0x6a48c0_length{0x40};
    std::vector<uint32_t> unknown_field_0x6a48c0(unknown_field_0x6a48c0_length);
    for(uint32_t i{}; i < unknown_field_0x6a48c0_length; ++i)
    {
      unknown_field_0x6a48c0[i] =
          overall_save_file_->GetAndIncrement<std::endian::big, uint32_t>(ptr);
    }

    // Now we have the font selection in the save file
    font_choice_ = overall_save_file_->GetAndIncrement<std::endian::big, uint32_t>(ptr);

    uint16_t font_size{overall_save_file_->GetAndIncrement<std::endian::big, uint16_t>(ptr)};
    if(font_size == 0)
    {
      if(!font_name_.data())
      {
        font_name_ = {};
      }
    }

    else
    {
      std::span<const std::byte> font_name_bytes = overall_save_file_->Get(ptr, font_size);
      font_name_ = Utility::ConvertShiftJISToUTF8String(font_name_bytes);
      ptr += font_size;
    }
  }

  catch(Utility::MappedFile::create_file_exception &e)
  {
    screen_mode_ = ScreenMode::WINDOWED;
    visible_ = true;
  }

  // TODO Windows to SDL config shit for window showing, cursor placement, window size etc.
}

void FVP::OpenHCBFile()
{
  hcb_file_ = std::make_unique<Utility::MappedFile>(
      std::vformat("{}/Snow.hcb", std::make_format_args(data_directory_)),
      Utility::MappedFile::Permissions::READ, Utility::MappedFile::CreateFile::NO_CREATE_FILE);

  // Syscall stuff, this is where they start
  hcb_current_file_position_ =
      hcb_file_->Get<std::endian::big, uint32_t>(hcb_current_file_position_);

  // Some number, idk yet what it represents
  uint32_t foo{hcb_file_->GetAndIncrement<std::endian::big, uint32_t>(hcb_current_file_position_)};

  // I need to confirm this, i am less sure.
  opcode_count_ =
      hcb_file_->GetAndIncrement<std::endian::big, uint16_t>(hcb_current_file_position_);
  opcodes_processed_ =
      hcb_file_->GetAndIncrement<std::endian::big, uint16_t>(hcb_current_file_position_);
  // opcodes_.reserve(opcode_count_ + opcodes_processed_);

  uint8_t game_mode_resolution_key{
      hcb_file_->GetAndIncrement<std::endian::big, uint8_t>(hcb_current_file_position_)};

  if(game_mode_resolution_key >= 0x10 || game_mode_resolution_key < 0x0)
  {
    std::runtime_error(
        "Failed to get game resolution from HCB file, key was outside range. Key value is " +
        std::to_string(game_mode_resolution_key));
  }

  else
  {
    window_width_ = WIDTH_HEIGHT_LOOKUP_TABLE[game_mode_resolution_key].width;
    window_height_ = WIDTH_HEIGHT_LOOKUP_TABLE[game_mode_resolution_key].height;
  }

  uint8_t game_mode_reserved{
      hcb_file_->GetAndIncrement<std::endian::big, uint8_t>(hcb_current_file_position_)};
  uint8_t game_title_size{
      hcb_file_->GetAndIncrement<std::endian::big, uint8_t>(hcb_current_file_position_)};

  std::span<const std::byte> game_title_bytes{
      hcb_file_->Get(hcb_current_file_position_, game_title_size)};

  game_title_ = Utility::ConvertShiftJISToUTF8String(game_title_bytes);
  hcb_current_file_position_ += game_title_size;

  // Now we have all the sys calls
  uint16_t syscall_count{
      hcb_file_->GetAndIncrement<std::endian::big, uint16_t>(hcb_current_file_position_)};

  // For now
  std::vector<SyscallEntry> syscall_table(syscall_count);

  for(uint16_t i{}; i < syscall_count; ++i)
  {
    syscall_table[i].argument_count =
        hcb_file_->GetAndIncrement<std::endian::big, uint8_t>(hcb_current_file_position_);
    syscall_table[i].name_length =
        hcb_file_->GetAndIncrement<std::endian::big, uint8_t>(hcb_current_file_position_);

    const auto name = hcb_file_->Get(hcb_current_file_position_, game_title_size);
    syscall_table[i].name =
        std::string_view(reinterpret_cast<const char *>(name.data()), name.size());
  }

  syscall_table_ = std::move(syscall_table);

  // Custom syscall count
  uint16_t custom_syscall_count{
      hcb_file_->GetAndIncrement<std::endian::big, uint16_t>(hcb_current_file_position_)};
}

void FVP::OpenWindowAndCursor()
{
  // Default system cursor is option 0
  cursors_[0] = std::make_unique<Cursor>(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));

  for(size_t cursor_num{1}; cursor_num < cursors_.size(); ++cursor_num)
  {
    try
    {
      Utility::MappedFile cursor(
          std::vformat("{}/cursor{}.ani", std::make_format_args(data_directory_, cursor_num)),
          Utility::MappedFile::Permissions::READ, Utility::MappedFile::CreateFile::NO_CREATE_FILE);
      auto cursor_data = cursor.Get(0, cursor.Data().size());
      cursors_[cursor_num] = std::make_unique<Cursor>(cursor_data);
    }
    catch(Utility::MappedFile::create_file_exception &e)
    {
      cursors_[cursor_num] = nullptr;
    }
  }

  rendering_window_ = std::make_unique<RenderWindow>(
      std::string_view(reinterpret_cast<const char *>(game_title_.data()), game_title_.size()),
      window_width_, window_height_,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | (visible_ ? 0 : SDL_WINDOW_HIDDEN));

  if(current_cursor_ != nullptr)
  {
    current_cursor_->get()->SetCursor();
  }
}

} // namespace Core
} // namespace fvp
