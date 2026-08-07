#pragma once

#include <cstddef>
#include <span>
#include <stdexcept>
#include <fcntl.h>

#include "util/binary_stream_util.hpp"

namespace fvp
{

namespace Formats
{

class PersistentFile
{
private:
  struct FileInfo
  {
    int file_descriptor;
    size_t file_size;
  };

  using BinaryStream = std::span<std::byte>;
  using ConstBinaryStream = std::span<const std::byte>;

  BinaryStream data_;
  FileInfo file_info_;
  std::string path_;
  int file_options_;
  bool is_mmaped_;

  // This is used for when dynamically allocated files need to be expanded to a bigger region of memory for writing purposes. This will also work for mmaped files by unmapping and remapping the data. TODO Scaling on multiplier 
  void Expand();

  // TODO We need to handle if file doesnt exist with options, similar to CreateFileA in windows. 
  // Basically handle if file dna and user species open only and not create, return null optional etc. 
  // Also, handle writing to memory in a good way (e.g. size increases). mmap case is very easy (allocate large virtual space)
  // Heap case is harder, but can just alloc large space for the file and realloc sparingly if we need to... 
public: 
  PersistentFile(const std::string_view, int file_options = O_RDONLY);
  ~PersistentFile();
  PersistentFile(const PersistentFile &) = delete;
  PersistentFile(PersistentFile &&) = delete;
  PersistentFile &operator=(const PersistentFile &) = delete;
  PersistentFile &operator=(PersistentFile &&) = delete;

  inline bool Valid() noexcept { return (!data_.empty()); }

  inline ConstBinaryStream Data() const noexcept { return data_; }

  // Really should be a size_t parameter, but not sure if I will need to use integers, shorts etc. This way atleast it allows those types and allows for checks to ensure no negative values
  template <Gettable T, typename P> [[nodiscard]] inline T Get(P offset) const
  {
    static_assert(std::is_integral_v<P>);
    if(offset < 0) return {};
    return Utility::Get<T>(data_, offset); 
  } 

  template <typename P> [[nodiscard]] inline ConstBinaryStream Get(P offset, size_t size) const
  {
    static_assert(std::is_integral_v<P>);
    if(offset < 0) return {};
    return Utility::Get(data_, offset, size); 
  }

  template <Gettable T, typename P> [[nodiscard]] inline T GetAndIncrement(P &offset) const
  {
    static_assert(std::is_integral_v<P>);
    T value{Get<T>(offset)};
    offset += sizeof(T);
    return value;
  }
  
  template <Gettable T> inline void Write(size_t offset, const T &value)
  {
    try
    {
      Utility::Write<T>(data_, offset, value);
    }
    // Only out of range error, on error, expand the file and then call Write() again. the stream shouldn't be nullptr *probably
    catch (std::out_of_range &r)
    {
      Expand();
      Write(offset, value);
    } 
  }
};
} // namespace Formats

} // namespace fvp
