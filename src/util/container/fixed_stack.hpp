#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace fvp
{

namespace Utility
{

// This is what I interpret to be the way the History in the game works with regards to speakers and the text they say (e.g. the backlog)
template<typename T, size_t Size>
class FixedStack
{
public:
  using value_type = T;
  using FixedBuffer = T[Size];

private:
  // We use a C-style array as the buffer on the stack
  FixedBuffer buffer_;
  size_t current_head_;
  size_t currently_filled_;
  
public:
  FixedStack() : buffer_(), current_head_(), currently_filled_() {}
  // For Save stuff to just create one from known values
  FixedStack(FixedBuffer &&buffer, size_t current_head, size_t currently_filled) : buffer_(std::move(buffer)), current_head_(current_head), currently_filled_(currently_filled) {}
  ~FixedStack() = default;
  FixedStack(const FixedStack &other) = default;
  FixedStack &operator=(const FixedStack &other) = default;
  FixedStack(FixedStack &&other) noexcept = default;
  FixedStack &operator=(FixedStack &&other) noexcept = default; 

  // This Operator gets the amount back from the head (e.g. a stack), So if head is at index 4, and we use [1] here, we get value at index 3 
  // It does not perform bounds checking in regards to what is currently filled, it does however convert indices that must wrap around the buffer (e.g. [10] at head=5 must go wrap around)
  constexpr T& operator[](size_t index) noexcept
  {
    size_t index_to_grab{current_head_};
    if(index > index_to_grab)
    {
      index_to_grab += Size; 
    } 

    return buffer_[(index_to_grab - index) % Size];
  }
  
  constexpr const T& operator[](size_t index) const noexcept
  {
    size_t index_to_grab{current_head_};
    if(index > index_to_grab)
    {
      index_to_grab += Size; 
    } 

    return buffer_[(index_to_grab - index) % Size];
  }

  constexpr T& at(size_t index)
  {
    if (index >= currently_filled_)
    {
      throw std::out_of_range("index supplied is greater than buffer size");
    }
    return operator[](index);
  }

  constexpr const T& at(size_t index) const
  {
    if (index >= currently_filled_)
    {
      throw std::out_of_range("index supplied is greater than buffer size");
    }
    return operator[](index);
  }

  // Copy the object passed to the stack
  constexpr void push(const T& object)
  {
    current_head_ = (current_head_ + 1) % Size; 
    currently_filled_ = currently_filled_ + (currently_filled_ != Size); 
    buffer_[current_head_] = object;
  }

  // Construct an object in place using the arguments passed
  template<typename ...Args>
  constexpr void emplace(Args&&... args)
  {
    current_head_ = (current_head_ + 1) % Size; 
    currently_filled_ = currently_filled_ + (currently_filled_ != Size);
    std::construct_at(&buffer_[current_head_], std::forward<Args>(args)...); 
  } 

  // Move an object to the stack
  constexpr void emplace(T &&object)
  {
    current_head_ = (current_head_ + 1) % Size; 
    currently_filled_ = currently_filled_ + (currently_filled_ != Size); 
    buffer_[current_head_] = std::move(object);
  }

  constexpr void pop()
  {
    if(currently_filled_ == 0)
    {
      throw std::out_of_range("Attempting to pop a value from empty fixed stack!");
    }

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

    currently_filled_--;
  };

  constexpr T top() const
  {
    return buffer_[current_head_];
  } 

  size_t size() const 
  {
    return currently_filled_;
  }
};
}
}
