#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "binary_stream_util.hpp"

template <class T>
concept Constructable = std::is_move_constructible_v<T> &&
                        std::convertible_to<std::vector<std::byte>, T> && requires(T t) {
                          { t.size() } -> std::convertible_to<size_t>;
                        };

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

class Image
{
  friend class HZC_Stream;

private:
  struct ImageMetaData
  {
    uint16_t type;
    uint16_t width;
    uint16_t height;
    uint16_t offset_x;
    uint16_t offset_y;
    uint16_t bpp;
    uint32_t unpacked_size;
    uint32_t header_size;
  };
  ImageMetaData meta_data_;
  std::unique_ptr<ImageBuf> view_{nullptr};
  std::unique_ptr<std::istream> base_{nullptr};
  std::unique_ptr<std::istream> stream_{nullptr};

public:
  Image(){};
  inline void PrintImageMetaData()
  {
    auto print_all = [](const auto &...params) { ((std::cout << params << "\n"), ...); };

    print_all(meta_data_.unpacked_size, meta_data_.header_size, meta_data_.type, meta_data_.width,
              meta_data_.height, meta_data_.offset_x, meta_data_.offset_y, meta_data_.bpp);
  }
};

class HZC_Stream
{
private:
  std::vector<std::byte> stream_;
  Image image_;

  template <typename T> HZC_Stream(T &&stream) : stream_(std::forward<T>(stream))
  {
    PrepareImage();
  }
  void PrepareImage();

public:
  template <Constructable T> static std::optional<HZC_Stream> Construct(T &&stream)
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

    return HZC_Stream(std::forward<T>(stream));
  }

  ~HZC_Stream() = default;
  HZC_Stream(const HZC_Stream &other) = delete;
  HZC_Stream &operator=(const HZC_Stream &other) = delete;
  HZC_Stream(HZC_Stream &&other) = default;
  HZC_Stream &operator=(HZC_Stream &&other) = default;
  inline void PrintImageMetaData() { image_.PrintImageMetaData(); }
  inline std::shared_ptr<char> GetImage()
  {
    size_t size = image_.meta_data_.unpacked_size;
    std::shared_ptr<char> buf(new char[size]);
    image_.stream_.get()->read(buf.get(), size);
    return buf;
  }
};

} // namespace Image

} // namespace AstralAir
