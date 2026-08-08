#pragma once

#include <cstdint>

namespace fvp
{

namespace Core
{

struct Opcode
{
  uint32_t arguments;
  uint8_t idk;
  uint8_t idk2;
  uint8_t idk3;
  uint8_t command;
};

} // namespace Core
} // namespace fvp
