#pragma once

#include "formats/save_information.hpp"
#include "opcode.hpp"
#include "util/persistent_file.hpp"
#include <cstdint>
#include <memory>
#include <string_view>

namespace fvp
{

namespace Core
{

class FVP
{
private:
  std::string_view save_file_directory_;
  std::string_view data_directory_;
  std::unique_ptr<Utility::PersistentFile> overall_save_file_;
  std::unique_ptr<Utility::PersistentFile> hcb_file_;
  // This might lead to a bug, saves should start at 0 but my own saves start at 1 hmm
  std::array<Formats::SaveInformation, 999> save_data_array_;
  std::vector<Opcode> opcodes_;
  uint32_t hcb_current_file_position_{};
  uint32_t save_preview_width_{};
  uint32_t save_preview_height_{};
  uint32_t preview_save_image_width_{};
  uint32_t preview_save_image_height_{};
  uint16_t opcode_count_{};
  uint16_t opcodes_processed_{};

  void OpenOverallSave();
  void OpenHCBFile();
  void GetSaveInformation(uint32_t save_number);
  FVP();

public:
  static FVP Init();
};
} // namespace Core
} // namespace fvp
