#include "file_view.hpp"

#include <cstdint>
#include <vector>

namespace AstralAir
{

namespace Data
{
class AstralAirData;
}

namespace Formats
{

class BinFormat
{
private:
  enum FileType
  {
    VOICE,
    BGM,
    GRAPH,
  };

  uint32_t count_;
  uint64_t index_size_;
  uint32_t data_name_size_;
  View file_view_;
  FileType file_type_;

public:
  BinFormat(const std::string &path);
  virtual ~BinFormat() = default;
  BinFormat(const BinFormat &other) = delete;
  BinFormat(BinFormat &&other) = default;
  BinFormat &operator=(const BinFormat &other) = delete;
  BinFormat &operator=(BinFormat &&other) = default;

  std::vector<AstralAir::Data::AstralAirData> OpenAndRead();
};

} // namespace Formats

} // namespace AstralAir
