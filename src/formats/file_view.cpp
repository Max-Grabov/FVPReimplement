#include "file_view.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <type_traits>
#include <vector>

namespace AstralAir
{

namespace Formats
{
View::View(const std::string &path) : file_name_(path), file_data_(path, std::ios::binary)
{
  byte_size_ = GetFileSize();
}

View::~View() { file_data_.close(); }

const std::ifstream &View::GetFileStream() const { return file_data_; }

/* A function designed to read a a type T, the template parameter. It will read up to sizeof(T)
 * bytes, starting at a byte offset dictated by the function parameter. This is meant to be a quick
 * use function when you know you need a certain data type (e.g. uint32_t) The other parameter is
 * the strategy used via a std::function. This is meant to dictate how you want to sort the bytes at
 * the end (i.e. Big vs Little endian). Ofcourse if you supply some lambda or function that makes no
 * sense, there is no guarantee the data will be safe and behavior is defined. The bare minimum is
 * that this function must return nothing and take a std::vector<std::byte> & as a parameter. By
 * default this strategy is null
 */
template <typename T>
T View::Read(const uint64_t offset, const std::function<void(std::vector<std::byte> &)> &strategy)
{
  static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>,
                "Type can't be reinterpret casted");

  if(!ValidPath())
  {
    return 0;
  }

  if(sizeof(T) * offset > byte_size_)
  {
    std::cerr << "Requested offset will be out of bounds, requested read is byte " << offset
              << "\n";
    return 0;
  }

  if(sizeof(T) * (offset + static_cast<T>(sizeof(T))) > byte_size_)
  {
    std::cerr << "Requested read will be out of bounds, requested read is bytes " << offset
              << " to " << offset + sizeof(T) << "\n";
    return 0;
  }

  std::vector<std::byte> buffer(sizeof(T));

  // We set the offset to what our current set offset is plus the parameter
  // Conversion is needed if our data type is bigger than 1 byte, as seekg offset acts as one byte
  // (e.g. uint64_t offset value of corresponds to an actual uint64_t 8 byte offset, whereas uint8_t
  // offset value should just be 1 byte per offset value)
  file_data_.seekg(sizeof(T) * offset, std::ios::beg);

  // This must be char *, uint8_t is not guarranteed to play nice (e.g. in testing all reads were
  // returning 0)
  file_data_.read(reinterpret_cast<char *>(buffer.data()), sizeof(T));
  file_data_.seekg(0, std::ios::beg);

  T data{};

  // Apply the user supplied strategy if it is non null
  if(strategy == nullptr)
  {
    std::memcpy(&data, buffer.data(), sizeof(T));
    return data;
  }
  strategy(buffer);
  std::memcpy(&data, buffer.data(), sizeof(T));
  return data;
}

/* Reads a buffer from a file, with the amount of bytes dictated by size
 * returns an empty buffer if the read is over bounds by size of offset
 * Or if the file view is invalid. Returns as a std vector of std bytes
 *
 * Similar to the other read function, users can specify a strategy to use for little or big endian
 */
std::vector<std::byte> View::Read(const uint64_t offset, const uint64_t size,
                                  const std::function<void(std::vector<std::byte> &)> &strategy)
{
  if(!ValidPath())
  {
    return {};
  }

  if(offset > byte_size_)
  {
    std::cerr << "Requested offset will be out of bounds, requested read is byte " << offset
              << "\n";
    return {};
  }

  if(offset + size > byte_size_)
  {
    std::cerr << "Requested read will be out of bounds, requested read is bytes " << offset
              << " to " << offset + size << "\n";
    return {};
  }

  // We write into a vector buffer of bytes
  std::vector<std::byte> buffer(size);
  file_data_.seekg(offset, std::ios::beg);
  file_data_.read(reinterpret_cast<char *>(buffer.data()), size);
  file_data_.seekg(0, std::ios::beg);

  // RVO
  if(strategy == nullptr)
    return buffer;
  strategy(buffer);
  return buffer;
}

uint64_t View::GetFileSize() const
{

  if(!ValidPath())
  {
    return 0;
  }

  struct stat file_stat;
  if(stat(file_name_.c_str(), &file_stat) < 0)
  {
    std::cerr << "Error getting file information for " << file_name_ << "\n";
    return 0;
  }

  return static_cast<uint64_t>(file_stat.st_size);
}

bool View::ValidPath() const { return file_data_.is_open(); }

template uint8_t
View::Read<uint8_t>(const uint64_t,
                    const std::function<void(std::vector<std::byte> &)> & = nullptr);
template uint32_t
View::Read<uint32_t>(const uint64_t,
                     const std::function<void(std::vector<std::byte> &)> & = nullptr);
template uint64_t
View::Read<uint64_t>(const uint64_t,
                     const std::function<void(std::vector<std::byte> &)> & = nullptr);

} // namespace Formats

} // namespace AstralAir
