#pragma once

#include "cursor.hpp"
#include "engine/save_information.hpp"
#include "engine/screen_mode.hpp"
#include "engine/syscall_entry.hpp"
#include "render_window.hpp"
#include "util/file/mapped_file.hpp"

#include <cstdint>
#include <memory>
#include <string_view>

namespace fvp
{

namespace Core
{

class FVP
{
public:
  void Run();

private:
  std::string_view save_file_directory_;
  std::string_view data_directory_;
  std::unique_ptr<Utility::MappedFile> overall_save_file_{nullptr};
  std::unique_ptr<Utility::MappedFile> hcb_file_{nullptr};
  std::unique_ptr<RenderWindow> rendering_window_{nullptr};
  std::array<Core::SaveInformation, 999> save_data_array_;
  std::array<std::unique_ptr<Cursor>, 4> cursors_{};
  std::vector<SyscallEntry> syscall_table_;
  std::vector<std::byte> game_title_;
  std::vector<std::byte> font_name_;
  std::unique_ptr<Cursor> *current_cursor_{nullptr};
  uint32_t hcb_current_file_position_{};
  uint32_t save_preview_width_{};
  uint32_t save_preview_height_{};
  uint32_t preview_save_image_width_{};
  uint32_t preview_save_image_height_{};
  uint32_t font_choice_{};
  uint16_t window_height_{};
  uint16_t window_width_{};
  uint16_t opcode_count_{};
  uint16_t opcodes_processed_{};
  uint8_t cursor_choice_{};
  bool visible_{};
  ScreenMode screen_mode_{};
  ScreenMode screen_mode_to_change_to{};

  void OpenOverallSave();
  void OpenHCBFile();
  void GetSaveInformation(uint32_t save_number);
  void InitializeData();
  void OpenWindowAndCursor();

public:
  FVP();
};
} // namespace Core
} // namespace fvp
