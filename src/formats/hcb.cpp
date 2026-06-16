#include "hcb.hpp"

namespace AstralAir
{

namespace Formats
{
void HcbFormat::OpenAndRead()
{
  meta_data_.script_length = file_view_.Read<uint32_t>(0);  
  meta_data_.script_start = file_view_.Read<uint32_t>(meta_data_.script_length); 
}
}
}
