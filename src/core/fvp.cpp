#include "fvp.hpp"
#include "util/file_view.hpp"

#include <cstdint>
#include <format>

namespace fvp
{

namespace Core
{

FVP::FVP() : save_file_directory_(""), overall_save_file_(std::vformat("{}/save/save.bin", std::make_format_args(save_file_directory_))), HCB_file_("") 
{}
void FVP::OpenOverallSave()
{
  // TODO Check if first time file opening as different instructions will happen  
  uint32_t ptr{};
  // TODO HCB stuff for opcode portion of the copies, also creating the opcode list 
  // TODO Windows to SDL config shit for window showing, cursor placement, window size etc.

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
