#include "hzc_stream.hpp"
#include "binary_stream_util.hpp"

namespace AstralAir
{

namespace Image
{

Image HZC_Stream::GetImage()
{
  Image image;
  image.meta_data_.unpacked_size = Utility::Get<uint32_t>(stream_, 4);
  image.meta_data_.header_size = Utility::Get<uint32_t>(stream_, 8);
  image.meta_data_.type = Utility::Get<uint16_t>(stream_, 18);
  image.meta_data_.width = Utility::Get<uint16_t>(stream_, 20);
  image.meta_data_.height = Utility::Get<uint16_t>(stream_, 22);
  image.meta_data_.offset_x = Utility::Get<uint16_t>(stream_, 24);
  image.meta_data_.offset_y = Utility::Get<uint16_t>(stream_, 26);

  return image;
}
}
}
