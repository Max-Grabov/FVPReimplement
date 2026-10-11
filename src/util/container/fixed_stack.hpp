#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>

namespace fvp
{

namespace Utility
{

template<typename T, std::size_t Size>
class FixedStack
{
public:
  using value_type = T;
  using FixedBuffer = T[Size];

private:
  // We use a C-style array as the buffer on the stack
  FixedBuffer buffer_;
  std::size_t current_head_;
  
public:
  FixedStack() : buffer_(), current_head_() {}

  // This Operator gets the amount back from the head (e.g. a stack), So if head is at index 4, and we use [1] here, we get value at index 3 
  constexpr T& operator[](std::size_t index) const
  {
    if (index >= Size)
    {
      throw std::out_of_range("index supplied is greater than buffer size");
    }

    std::size_t index_to_grab{current_head_};
    if(index > index_to_grab)
    {
      index_to_grab += Size; 
    } 

    return buffer_[(index_to_grab - index) % Size];
  }

  constexpr void push()
  {
    current_head_ = (current_head_ + 1) % Size; 
    buffer_[current_head_] = {};
  }

  template<typename ...Args>
  constexpr void push(Args&&... args)
  {
    current_head_ = (current_head_ + 1) % Size; 
    std::construct_at(&buffer_[current_head_], std::forward<Args>(args)...); 
  } 

  constexpr void pop()
  {
    buffer_[current_head_] = {}; 

    // Wrap around if value is at 0
    if (current_head_ >= 0)
    {
      current_head_--; 
    }
    else
    {
      current_head_ = Size - 1;
    }
  };

  constexpr T top() const
  {
    return buffer_[current_head_];
  }  
};
}
}
