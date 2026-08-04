#pragma once
#include "image.hpp"

#include <cstddef>
#include <optional>
#include <streambuf>
#include <vector>
#include <span>

namespace fvp
{

namespace Image
{

class ImageBuf : public std::streambuf
{
public:
  inline ImageBuf(char *begin, char *end) { setg(begin, begin, end); }
  inline ImageBuf() {}
};

[[nodiscard]]
std::optional<Image> CreateImage(std::vector<std::byte> &&stream);

[[nodiscard]]
std::optional<Image> CreateImageFromRawBytes(std::span<const std::byte> &stream, uint16_t width, uint16_t height);

} // namespace Image
} // namespace fvp
