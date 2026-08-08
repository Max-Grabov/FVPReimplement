#pragma once

#include <algorithm>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <span>
#include <stdexcept>
#include <type_traits>

template <typename T>
concept Gettable = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

template <typename T>
concept EndianSwappable = std::is_integral_v<T> || std::is_floating_point_v<T>;

namespace fvp
{

namespace Utility
{

// Returns a value that is memcopied from the span's pointed memory at the offset specified.
template <Gettable T> [[nodiscard]] T Get(const std::span<const std::byte> &stream, size_t offset)
{
  if(!stream.data())
  {
    throw std::runtime_error("Null Stream");
  }

  if(offset > stream.size())
  {
    throw std::out_of_range("Offset is larger than size");
  }

  if(offset + sizeof(T) > stream.size())
  {
    throw std::out_of_range("Offset + template size is larger than size");
  }

  T data{};

  std::memcpy(&data, stream.data() + offset, sizeof(T));
  return data;
}

[[nodiscard]] inline std::span<const std::byte> Get(const std::span<const std::byte> &stream,
                                                    size_t offset, size_t size)
{
  if(!stream.data())
  {
    throw std::runtime_error("Null Stream");
  }

  if(offset > stream.size())
  {
    throw std::out_of_range("Offset is larger than size");
  }

  if(offset + size > stream.size())
  {
    throw std::out_of_range("Offset + template size is larger than size");
  }

  return std::span<const std::byte>(stream.data() + offset, size);
}

// Returns a std span at the specified offset and size of the data (e.g. good for strings)

template <Gettable T> void Write(std::span<std::byte> stream, size_t offset, const T &value)
{
  if(!stream.data())
  {
    throw std::runtime_error("Null Stream");
  }

  if(offset > stream.size())
  {
    throw std::out_of_range("Offset is larger than size");
  }

  if(offset + sizeof(T) > stream.size())
  {
    throw std::out_of_range("Offset + template size is larger than size");
  }

  std::memcpy(stream.data() + offset, &value, sizeof(T));
}

template <std::endian E> void ConvertToEndian(std::span<std::byte> &stream)
{
  if constexpr(std::endian::native == E)
    return;
  std::reverse(stream.begin(), stream.end());
}

template <std::endian E, EndianSwappable T> void ConvertToEndian(T &value)
{
  std::reverse(reinterpret_cast<std::byte *>(&value),
               reinterpret_cast<std::byte *>(&value) + sizeof(T));
}

template <std::endian E, EndianSwappable T> [[nodiscard]] T ConvertToEndian(T &&value)
{
  std::reverse(reinterpret_cast<std::byte *>(&value),
               reinterpret_cast<std::byte *>(&value) + sizeof(T));
  return value;
}

inline void PrintAsString(const std::span<const std::byte> &stream)
{
  for(const auto &b : stream)
  {
    std::cout << static_cast<char>(b);
  }
  std::cout << "\n";
}

} // namespace Utility
} // namespace fvp
