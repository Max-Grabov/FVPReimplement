#pragma once

#include <audio_stream.hpp>
#include <cstddef>
#include <optional>
#include <vector>

namespace AstralAir
{

namespace Audio
{

std::optional<AudioStream> DecodeWAV(std::vector<std::byte> &&);

std::optional<AudioStream> DecodeOggContainer(const std::vector<std::byte> &);

} // namespace Audio
} // namespace AstralAir
