#pragma once

#include <cstdint>

namespace fvp
{

namespace Utility
{

template<typename T, std::size_t Size>
class CircularBuffer
{
public:
  using value_type = T;
  using FixedBuffer = T[Size];

private:
  // We use a C-style array as the buffer on the stack
  FixedBuffer buffer_;
  std::size_t current_head_;
  
public:
  CircularBuffer()
};
}
}
