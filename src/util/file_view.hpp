#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <type_traits>

namespace fvp
{

namespace Utility
{

class View
{
private:
  uint64_t byte_size_;
  std::string file_name_;
  std::ifstream file_data_;

public:
  View(const std::string &path) : file_name_(path), file_data_(path, std::ios::binary)
  {
    byte_size_ = GetFileSize();
  }

  ~View() { file_data_.close(); }

  View(View &) = delete;
  View &operator=(const View &) = delete;
  View(View &&) = default;
  View &operator=(View &&) = default;

  const std::ifstream &GetFileStream() const { return file_data_; }

  template <typename T>
  T Read(const uint64_t offset, const std::function<void(std::vector<std::byte> &)> &strategy = nullptr)
  {
    if(!ValidPath())
    {
      throw std::runtime_error("Provided file path does not exist! Provided path is: " + file_name_);
    }

    if(offset > byte_size_)
    {
      throw std::runtime_error("Requested offset will be out of bounds, requested read is byte " +
                               std::to_string(offset));
    }

    if(offset + static_cast<T>(sizeof(T)) > byte_size_)
    {
      throw std::runtime_error("Requested read will be out of bounds, requested read is bytes " +
                               std::to_string(offset) + " to " + std::to_string(offset + sizeof(T)));
    }

    std::vector<std::byte> buffer(sizeof(T));

    file_data_.seekg(offset, std::ios::beg);

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

  std::vector<std::byte> Read(const uint64_t offset, const uint64_t size,
                                  const std::function<void(std::vector<std::byte> &)> &strategy = nullptr)
  {
    if(!ValidPath())
    {
      throw std::runtime_error("Provided file path does not exist! Provided path is: " + file_name_);
    }

    if(offset > byte_size_)
    {
      throw std::runtime_error("Requested offset will be out of bounds, requested read is byte " +
                               std::to_string(offset));
    }

    if(offset + size > byte_size_)
    {
      throw std::runtime_error("Requested read will be out of bounds, requested read is bytes " +
                               std::to_string(offset) + " to " + std::to_string(offset + size));
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

  uint64_t GetFileSize() const
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

  bool ValidPath() const { return file_data_.is_open(); }
};

} // namespace Formats

} // namespace fvp

