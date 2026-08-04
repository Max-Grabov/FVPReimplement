#include "fvp.hpp"

#include <cstdint>
#include <format>
#include <span>

namespace fvp
{

namespace Core
{

// TODO IMPLEMENT FIND FILE, THIS IS TEMPORARY FOR TESTING!
FVP::FVP() : save_file_directory_("./AstralAirData"), data_directory_("./AstralAirData"), overall_save_file_(std::vformat("{}/save/save.bin", std::make_format_args(save_file_directory_))), HCB_file_(std::vformat("{}/Snow.hcb", std::make_format_args(data_directory_))) 
{}

// TODO for now
FVP FVP::Init()
{
  return FVP();
}

void FVP::OpenOverallSave()
{
  if(overall_save_file_.Valid())
  {
    uint32_t ptr{};
    memcpy(opcodes_.data() + opcode_count_, overall_save_file_.Data(), opcodes_processed_); 
    ptr += opcodes_processed_ * sizeof(Opcode);

    // These things need to get refactored into some SDL Handler later... Atleast some class should exist that can set these values. 
    uint8_t foo{overall_save_file_.GetAndIncrement<uint8_t>(ptr)};
    bool visible{static_cast<bool>(overall_save_file_.GetAndIncrement<uint8_t>(ptr))};
    
    uint32_t left_position{overall_save_file_.GetAndIncrement<uint32_t>(ptr)};
    uint32_t top_position{overall_save_file_.GetAndIncrement<uint32_t>(ptr)};
    
    // TODO Handle 2nd window stuff and right position + bottom
    uint8_t some_2nd_window_byte{overall_save_file_.GetAndIncrement<uint8_t>(ptr)}; 
    uint8_t field_0x81{overall_save_file_.GetAndIncrement<uint8_t>(ptr)}; 

    // Save preview image dimensions!!!
    preview_save_image_width_ = overall_save_file_.GetAndIncrement<uint32_t>(ptr);
    preview_save_image_height_ = overall_save_file_.GetAndIncrement<uint32_t>(ptr);
  }

  else
  {

  }
 
  // TODO HCB stuff for opcode portion of the copies, also creating the opcode list 
  // TODO Windows to SDL config shit for window showing, cursor placement, window size etc.
}

void FVP::OpenHCBFile()
{
  // Syscall stuff, this is where they start
  hcb_current_file_position_ = HCB_file_.Get<uint32_t>(hcb_current_file_position_);

  // Some number, idk yet what it represents
  uint32_t foo{HCB_file_.GetAndIncrement<uint32_t>(hcb_current_file_position_)};

  // I think
  opcode_count_ = HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_);
  opcodes_processed_ = HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_);

  opcodes_.reserve(opcode_count_ + opcodes_processed_);

  // I think this is what this byte represents, in the decomp it is read and used to scale the width, height, etc.
  uint16_t window_scaling_value{HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_)};
  uint16_t game_title_size{HCB_file_.GetAndIncrement<uint8_t>(hcb_current_file_position_)};
 
  const std::span<const std::byte> game_title{HCB_file_.Get(hcb_current_file_position_, game_title_size)};
  hcb_current_file_position_ += game_title_size;

  // Now we have all the sys calls
  uint16_t syscall_count{HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_)}; 

  // TODO This is not simple
  while(false)
  {
    if(syscall_count == 0)
    {
      
    }
  }
}

}
}
