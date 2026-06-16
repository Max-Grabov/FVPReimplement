#include "image_decoder.hpp"
#include "binary_stream_util.hpp"
#include "image.hpp"

#include "zstr.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace AstralAir
{

namespace Image
{

std::optional<Image> CreateImage(std::vector<std::byte> &&stream)
{
  // Check some obvious conditions
  // Metadata at a minimum 28 bytes
  if(stream.size() < 28)
  {
    return std::nullopt;
  }

  // All Streams should start with these 4 bytes (big endian representing 'hzc1')
  if(Utility::Get<uint32_t>(stream, 0) == 0x3d637a68)
  {
    return std::nullopt;
  }

  // At the position 12 + header_size is where the zlib header is located, this must be divisible by
  // 31 and is stored as a little endian 78 01 78 5E 78 9C 78 DA
  uint16_t zlib_header{Utility::Get<uint16_t>(stream, Utility::Get<uint32_t>(stream, 8) + 12)};
  Utility::ConvertToEndian<std::endian::little, uint16_t>(zlib_header);
  if((zlib_header % 31) != 0)
  {
    return std::nullopt;
  }
  // Metadata Information
  uint32_t unpacked_size = Utility::Get<uint32_t>(stream, 4);
  uint32_t header_size = Utility::Get<uint32_t>(stream, 8);
  uint16_t type = Utility::Get<uint16_t>(stream, 18);
  uint16_t width = Utility::Get<uint16_t>(stream, 20);
  uint16_t height = Utility::Get<uint16_t>(stream, 22);
  uint16_t offset_x = Utility::Get<uint16_t>(stream, 24);
  uint16_t offset_y = Utility::Get<uint16_t>(stream, 26);
  uint16_t bpp = 0 == type ? 24 : type > 2 ? 8 : 32;
  uint32_t position{12 + header_size};

  auto view = std::make_unique<ImageBuf>(reinterpret_cast<char *>(stream.data()) + position,
                                         reinterpret_cast<char *>(stream.data()) + stream.size());
  auto z_stream = std::make_unique<zstr::istream>(view.get());

  std::vector<char> pixels(unpacked_size);

  z_stream->read(pixels.data(), unpacked_size);
  return Image({type, width, height, offset_x, offset_y, bpp, unpacked_size, header_size},
               std::move(pixels));
}
} // namespace Image
} // namespace AstralAir
