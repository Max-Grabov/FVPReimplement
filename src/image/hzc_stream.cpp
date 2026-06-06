#include "hzc_stream.hpp"
#include "binary_stream_util.hpp"

#include "zstr.hpp"
#include <cstdint>
#include <memory>

namespace AstralAir
{

namespace Image
{

void HZC_Stream::PrepareImage()
{
  // Metadata Information
  image_.meta_data_.unpacked_size = Utility::Get<uint32_t>(stream_, 4);
  image_.meta_data_.header_size = Utility::Get<uint32_t>(stream_, 8);
  image_.meta_data_.type = Utility::Get<uint16_t>(stream_, 18);
  image_.meta_data_.width = Utility::Get<uint16_t>(stream_, 20);
  image_.meta_data_.height = Utility::Get<uint16_t>(stream_, 22);
  image_.meta_data_.offset_x = Utility::Get<uint16_t>(stream_, 24);
  image_.meta_data_.offset_y = Utility::Get<uint16_t>(stream_, 26);
  image_.meta_data_.bpp = 0 == image_.meta_data_.type ? 24 : image_.meta_data_.type > 2 ? 8 : 32;

  uint32_t position{12 + image_.meta_data_.header_size};
  image_.view_ =
      std::make_unique<ImageBuf>(reinterpret_cast<char *>(stream_.data()) + position,
                                 reinterpret_cast<char *>(stream_.data()) + stream_.size());
  image_.stream_ = std::make_unique<zstr::istream>(image_.view_.get());
}
} // namespace Image
} // namespace AstralAir
