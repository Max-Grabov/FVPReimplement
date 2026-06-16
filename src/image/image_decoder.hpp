#pragma once
#include "image.hpp"

#include <cstddef>
#include <optional>
#include <streambuf>
#include <vector>

namespace AstralAir
{

namespace Image
{

class ImageBuf : public std::streambuf
{
public:
  inline ImageBuf(char *begin, char *end) { setg(begin, begin, end); }
  inline ImageBuf() {}
};

std::optional<Image> CreateImage(std::vector<std::byte> &&stream);

} // namespace Image
} // namespace AstralAir
