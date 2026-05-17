#include <cstddef>
#include <cstdint>
#include <endian.h>
#include <fstream>
#include <vector>
#include <string>
#include <bit>

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
  View operator=(View &) = delete;
  View(View &&) = default;
  View &operator=(View &&) = default;

  const std::ifstream &GetFileStream() const;
  template <typename T> T Read(const uint64_t, std::endian);
  std::vector<std::byte> ReadBuffer(const uint64_t, const uint64_t);
  uint64_t GetFileSize() const;
  bool ValidPath() const;

private:
  uint64_t byte_size_;
  std::string file_name_;
  std::ifstream file_data_;
};

} // namespace Formats

} // namespace AstralAir
