#pragma once

#include <audio_stream.hpp>
#include <cstddef>
#include <optional>
#include <vector>

namespace fvp
{

namespace Audio
{

[[nodiscard]]
std::optional<AudioStream> DecodeWAV(std::vector<std::byte> &&);

[[nodiscard]]
std::optional<AudioStream> DecodeOggContainer(const std::vector<std::byte> &);

} // namespace Audio
} // namespace fvp
