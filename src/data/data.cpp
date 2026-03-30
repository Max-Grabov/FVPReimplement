#include "data.hpp"

namespace AstralAir
{

namespace Data
{

AstralAirData::AstralAirData(const std::string &name, uint32_t offset, uint32_t data)
    : data_name_(name), offset_(offset), data_(data)
{
}
} // namespace Data
} // namespace AstralAir
