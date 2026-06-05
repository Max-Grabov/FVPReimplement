#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>
#include <optional>

#include "image.hpp"


template <class T>
concept Constructable = std::is_move_constructible_v<T> && 
                        std::convertible_to<std::vector<std::byte>, T> &&
                        requires(T t) { { t.size() } -> std::convertible_to<size_t>; };

namespace AstralAir
{

namespace Image
{

class HZC_Stream
{
private:
  
  std::vector<std::byte> stream_;

  template <typename T>
  HZC_Stream(T && stream) : stream_(std::forward<T>(stream)) {}

public:
  template <Constructable T>
  std::optional<HZC_Stream> Construct(T && stream)
  {
    // Check some obvious conditions
    // Metadata at a minimum 28 bytes
    if(stream.size() < 28)
    {
      return std::nullopt;
    }

    return HZC_Stream(std::forward<T>(stream));
  } 

  ~HZC_Stream() = default;
  HZC_Stream(const HZC_Stream &other) = default;
  HZC_Stream& operator=(const HZC_Stream &other) = default;
  HZC_Stream(HZC_Stream &&other) = default;
  HZC_Stream& operator=(HZC_Stream &&other) = default; 

  Image GetImage(); 
};

}

}

