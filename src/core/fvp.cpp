#include "fvp.hpp"
#include "util/encoding/encoding.hpp"
#include "util/file/mapped_file.hpp"

#include <cstdint>
#include <format>
#include <memory>
#include <span>

namespace fvp
{

namespace Core
{

// TODO IMPLEMENT FIND FILE, THIS IS TEMPORARY FOR TESTING!
FVP::FVP() : save_file_directory_("./AstralAirData"), data_directory_("./AstralAirData") {}

// TODO for now
FVP FVP::Init() { return FVP(); }

void FVP::OpenOverallSave()
{
  try
  {
    overall_save_file_ = std::make_unique<Utility::MappedFile>(
        std::vformat("{}/save/save.bin", std::make_format_args(save_file_directory_)),
        Utility::MappedFile::Permissions::READ_WRITE,
        Utility::MappedFile::CreateFile::NO_CREATE_FILE);

    uint32_t ptr{};
    memcpy(opcodes_.data() + opcode_count_, overall_save_file_->Data().data(), opcodes_processed_);
    ptr += opcodes_processed_ * sizeof(Opcode);

    // These things need to get refactored into some SDL Handler later... Atleast some class should
    // exist that can set these values.
    uint8_t foo{overall_save_file_->GetAndIncrement<uint8_t>(ptr)};
    bool visible{static_cast<bool>(overall_save_file_->GetAndIncrement<uint8_t>(ptr))};

    uint32_t left_position{overall_save_file_->GetAndIncrement<uint32_t>(ptr)};
    uint32_t top_position{overall_save_file_->GetAndIncrement<uint32_t>(ptr)};

    // TODO Handle 2nd window stuff and right position + bottom
    uint8_t some_2nd_window_byte{overall_save_file_->GetAndIncrement<uint8_t>(ptr)};
    uint8_t field_0x81{overall_save_file_->GetAndIncrement<uint8_t>(ptr)};

    // Save preview image dimensions!!!
    preview_save_image_width_ = overall_save_file_->GetAndIncrement<uint32_t>(ptr);
    preview_save_image_height_ = overall_save_file_->GetAndIncrement<uint32_t>(ptr);
  }

  catch(Utility::MappedFile::create_file_exception &e)
  {
    // NEW FILE CASE
  }

  // TODO HCB stuff for opcode portion of the copies, also creating the opcode list
  // TODO Windows to SDL config shit for window showing, cursor placement, window size etc.
}

void FVP::OpenHCBFile()
{
  hcb_file_ = std::make_unique<Utility::MappedFile>(
      std::vformat("{}/Snow.hcb", std::make_format_args(data_directory_)),
      Utility::MappedFile::Permissions::READ, Utility::MappedFile::CreateFile::NO_CREATE_FILE);

  // Syscall stuff, this is where they start
  hcb_current_file_position_ = hcb_file_->Get<uint32_t>(hcb_current_file_position_);

  // Some number, idk yet what it represents
  uint32_t foo{hcb_file_->GetAndIncrement<uint32_t>(hcb_current_file_position_)};

  // I think
  opcode_count_ = hcb_file_->GetAndIncrement<uint16_t>(hcb_current_file_position_);
  opcodes_processed_ = hcb_file_->GetAndIncrement<uint16_t>(hcb_current_file_position_);

  opcodes_.reserve(opcode_count_ + opcodes_processed_);

  // I think this is what this byte represents, in the decomp it is read and used to scale the
  // width, height, etc.
  uint16_t window_scaling_value{hcb_file_->GetAndIncrement<uint16_t>(hcb_current_file_position_)};
  uint16_t game_title_size{hcb_file_->GetAndIncrement<uint8_t>(hcb_current_file_position_)};

  std::span<const std::byte> game_title{
      hcb_file_->Get(hcb_current_file_position_, game_title_size)};

  Utility::ConvertShiftJISToUTF8String(game_title);
  hcb_current_file_position_ += game_title_size;

  // Now we have all the sys calls
  uint16_t syscall_count{hcb_file_->GetAndIncrement<uint16_t>(hcb_current_file_position_)};

  // TODO This is not simple
  while(false)
  {
    if(syscall_count == 0)
    {
    }
  }
}

} // namespace Core
} // namespace fvp
