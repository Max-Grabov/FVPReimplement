#pragma once

#include "asset/formats/persistent_file.hpp"
#include "asset/formats/save_information.hpp"
#include <cstdint>
#include <string_view>

namespace fvp
{

namespace Core
{

class FVP
{
private:
  std::string_view save_file_directory_;
  Formats::PersistentFile overall_save_file_;
  Formats::PersistentFile HCB_file_;
  // This might lead to a bug, saves should start at 0 but my own saves start at 1 hmm
  std::array<Formats::SaveInformation, 999> save_data_array_;
  uint32_t save_preview_width_;
  uint32_t save_preview_height_;

  void OpenOverallSave();
  void GetSaveInformation(uint32_t save_number);
public:
  FVP(); 
};
}
}
