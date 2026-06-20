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
#include <vector>

template <typename T>
concept Gettable = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

template <typename T>
concept EndianSwappable = std::is_integral_v<T> || std::is_floating_point_v<T>;

namespace AstralAir
{

namespace Utility
{

template <Gettable T> T Get(const std::span<const std::byte> &stream, size_t offset)
{
  if(offset > stream.size())
  {
    throw std::runtime_error("Offset is larger than size");
  }

  if(offset + sizeof(T) > stream.size())
  {
    throw std::runtime_error("Offset + template size is larger than size");
  }

  T data{};

  std::memcpy(&data, stream.data() + offset, sizeof(T));
  return data;
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

template <std::endian E, EndianSwappable T> T ConvertToEndian(T &&value)
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
} // namespace AstralAir
