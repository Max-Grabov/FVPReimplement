#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace AstralAir
{

namespace Formats
{

class View
{
public:
  View(const std::string &);
  ~View();
  View(View &) = delete;
  View &operator=(const View &) = delete;
  View(View &&) = default;
  View &operator=(View &&) = default;

  const std::ifstream &GetFileStream() const;
  template <typename T>
  T Read(const uint64_t, const std::function<void(std::vector<std::byte> &)> & = nullptr);
  std::vector<std::byte> Read(const uint64_t, const uint64_t,
                              const std::function<void(std::vector<std::byte> &)> & = nullptr);
  uint64_t GetFileSize() const;
  bool ValidPath() const;

private:
  uint64_t byte_size_;
  std::string file_name_;
  std::ifstream file_data_;
};

} // namespace Formats

} // namespace AstralAir
