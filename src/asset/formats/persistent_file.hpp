#pragma once

#include <cstddef>
#include <span>
#include <string>

#include "util/binary_stream_util.hpp"

namespace fvp
{

namespace Formats
{

class PersistentFile
{
protected:
  using BinaryStream = std::span<const std::byte>;

  BinaryStream data_;
  bool is_mmaped_;

public:
  explicit PersistentFile(const std::string &);
  ~PersistentFile();
  PersistentFile(const PersistentFile &) = delete;
  PersistentFile(PersistentFile &&) = delete;
  PersistentFile &operator=(const PersistentFile &) = delete;
  PersistentFile &operator=(PersistentFile &&) = delete;

  template <Gettable T> [[nodiscard]] inline T Get(size_t offset)
  {
    return Utility::Get<T>(data_, offset); 
  } 
  
  template <Gettable T> inline void Write(size_t offset, const T &value)
  {
    Utility::Write<T>(data_, offset, value);
  }
};
} // namespace Formats

} // namespace fvp
