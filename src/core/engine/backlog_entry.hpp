#include <span>
#include <cstdint>
#include <cstddef>

namespace fvp
{

namespace Core
{

struct BacklogEntry
{
  std::span<const std::byte> speaker;
  std::span<const std::byte> spoken_text;
  uint32_t maybe_text_id;
};

} // namespace Core
} // namespace fvp
