#pragma once

#include <cstddef>
#include <span>

#include "util/binary_stream_util.hpp"

namespace fvp
{

namespace Formats
{

class PersistentFile
{
private:
  using BinaryStream = std::span<const std::byte>;

  BinaryStream data_;
  bool is_mmaped_;

  // TODO We need to handle if file doesnt exist with options, similar to CreateFileA in windows. 
  // Basically handle if file dna and user species open only and not create, return null optional etc. 
  // Also, handle writing to memory in a good way (e.g. size increases). mmap case is very easy (allocate large virtual space)
  // Heap case is harder, but can just alloc large space for the file and realloc sparingly if we need to... 
public: 
  explicit PersistentFile(const std::string_view);
  ~PersistentFile();
  PersistentFile(const PersistentFile &) = delete;
  PersistentFile(PersistentFile &&) = delete;
  PersistentFile &operator=(const PersistentFile &) = delete;
  PersistentFile &operator=(PersistentFile &&) = delete;

  inline bool Valid() { return (!data_.empty()); }

  inline const std::byte *Data() const { return data_.data(); }

  // Really should be a size_t parameter, but not sure if I will need to use integers, shorts etc. This way atleast it allows those types and allows for checks to ensure no negative values
  template <Gettable T, typename P> [[nodiscard]] inline T Get(P offset)
  {
    static_assert(std::is_integral_v<P>);
    if(offset < 0) return {};
    return Utility::Get<T>(data_, offset); 
  } 

  template <typename P> [[nodiscard]] inline BinaryStream Get(P offset, size_t size)
  {
    static_assert(std::is_integral_v<P>);
    if(offset < 0) return {};
    return Utility::Get(data_, offset, size); 
  }

  template <Gettable T, typename P> [[nodiscard]] inline T GetAndIncrement(P &offset)
  {
    static_assert(std::is_integral_v<P>);
    T value{Get<T>(offset)};
    offset += sizeof(T);
    return value;
  }
  
  template <Gettable T> inline void Write(size_t offset, const T &value)
  {
    Utility::Write<T>(data_, offset, value);
  }
};
} // namespace Formats

} // namespace fvp
