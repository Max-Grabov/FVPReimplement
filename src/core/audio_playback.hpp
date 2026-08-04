#pragma once
#include "audio/audio_stream.hpp"
#include <functional>

#include <SDL3/SDL_audio.h>

namespace fvp
{

namespace Core
{

SDL_AudioStream *CreateAudioStream(const Audio::AudioStream &);

void PlayAudio(SDL_AudioStream *, const Audio::AudioStream &,
               const std::function<void(SDL_AudioStream *, const Audio::AudioStream &)> & = nullptr);

} // namespace Audio
} // namespace fvp
