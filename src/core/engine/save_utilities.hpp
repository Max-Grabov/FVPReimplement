#pragma once

#include <cstddef>
#include <cstdint>

#include "backlog_entry.hpp"
#include "save_information.hpp"
#include "image/image.hpp"
#include "util/container/fixed_stack.hpp"
#include "util/file/mapped_file.hpp"

namespace fvp
{

namespace Core
{

inline SaveInformation GetSaveInformation(const Utility::MappedFile &save_file,
                                          size_t starting_byte);

inline Image::Image GetSavePreviewImage(const Utility::MappedFile &save_file, uint32_t width,
                                        uint32_t height, size_t starting_byte);

inline Utility::FixedStack<BacklogEntry, 100> GetBacklog(const Utility::MappedFile &save_file,
                                                         size_t starting_byte);
} // namespace Core
} // namespace fvp
