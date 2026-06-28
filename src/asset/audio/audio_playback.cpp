#include "audio_playback.hpp"
#include "audio_stream.hpp"

namespace fvp
{

namespace Audio
{

SDL_AudioStream *CreateAudioStream(const AudioStream &stream)
{
  SDL_AudioSpec spec;
  spec.channels = stream.GetChannels();
  spec.freq = stream.GetRate();
  spec.format = SDL_AUDIO_F32;

  // We handle null outside, (just not playing the audio that was going to be inputted via this
  // stream)
  return SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
}

void PlayAudio(
    SDL_AudioStream *stream, const AudioStream &data_stream,
    const std::function<void(SDL_AudioStream *, const AudioStream &)> &data_processing_strategy)
{
  // If no supplied strategy, we just dump all of the data into the stream at once
  if(!data_processing_strategy)
  {
    SDL_PutAudioStreamData(stream, data_stream.GetStream().data(),
                           data_stream.GetStream().size() * sizeof(float));
  }
  else
  {
    data_processing_strategy(stream, data_stream);
  }
}

} // namespace Audio
} // namespace fvp
