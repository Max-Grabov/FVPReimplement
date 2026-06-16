#include "hcb.hpp"

// Most code here was heavily inspired and implemented through here ->
// https://github.com/akerou/fvp/tree/master Most changes are repurposing code to fit the codebase
// and more modern c++ standards.
namespace AstralAir
{

namespace Formats
{
void HcbFormat::OpenAndRead()
{
  meta_data_.script_length = file_view_.Read<uint32_t>(0);
  meta_data_.script_start = file_view_.Read<uint32_t>(meta_data_.script_length);
}
} // namespace Formats
} // namespace AstralAir
