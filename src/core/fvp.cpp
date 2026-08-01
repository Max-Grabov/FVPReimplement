#include "fvp.hpp"
#include "util/file_view.hpp"

#include <cstdint>
#include <format>
#include <span>

namespace fvp
{

namespace Core
{

FVP::FVP() : save_file_directory_(""), overall_save_file_(std::vformat("{}/save/save.bin", std::make_format_args(save_file_directory_))), HCB_file_("") 
{}
void FVP::OpenOverallSave()
{
  if(overall_save_file_.Valid())
  {
    uint32_t ptr{};
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
  uint16_t opcode_count{HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_)};
  uint16_t opcodes_processed{HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_)};  

  opcodes_.reserve(opcode_count + opcodes_processed);

  // I think this is what this byte represents, in the decomp it is read and used to scale the width, height, etc.
  uint16_t window_scaling_value{HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_)};
  uint16_t game_title_size{HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_)};
 
  const std::span<const std::byte> game_title{HCB_file_.Get(hcb_current_file_position_, game_title_size)};
  hcb_current_file_position_ += game_title_size;

  // Now we have all the sys calls
  uint16_t syscall_count{HCB_file_.GetAndIncrement<uint16_t>(hcb_current_file_position_)}; 

  // TODO
  while(true)
  {
    if(syscall_count == 0)
    {
      
    }
  }
}

void FVP::GetSaveInformation(uint32_t save_number)
{
  // Should be equivalent to _sprintf(buffer, "%s/save/s%03d.bin", save_file_directory, save_number)
  // std format will pad the zeroes properly
  Utility::View save(std::vformat("{}/save/s{:03}.bin", std::make_format_args(save_file_directory_, save_number)));
  Formats::SaveInformation &save_info = save_data_array_.at(save_number);
  uint32_t ptr{0};

  save_info.year = save.Read<uint16_t>(ptr);
  ptr += 2; 
  save_info.month = save.Read<uint8_t>(ptr++);
  save_info.day = save.Read<uint8_t>(ptr++);
  save_info.day_of_week = save.Read<uint8_t>(ptr++);
  save_info.hour = save.Read<uint8_t>(ptr++);
  save_info.minute = save.Read<uint8_t>(ptr++);

  
  uint16_t size{};
  size = save.Read<uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    save_info.byte_stream_1 = save.Read(ptr, size + 1);
    save_info.byte_stream_1.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save.Read<uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    save_info.byte_stream_2 = save.Read(ptr, size + 1);
    save_info.byte_stream_2.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }

  size = save.Read<uint16_t>(ptr);
  ptr += 2;
  if(size != 0)
  {
    save_info.preview_text = save.Read(ptr, size + 1);
    save_info.preview_text.at(size) = static_cast<std::byte>(0);
    ptr += size;
  }
}
}
}
