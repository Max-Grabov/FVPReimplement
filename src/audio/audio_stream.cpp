#include "audio_stream.hpp"

namespace fvp
{
namespace Audio
{
/* Creates a new AudioStream object from a supplied pcm output stream, channels, and rate. This is
 * meant to be created after going through some vorbis container with an Ogg Stream inside of it
 * (e.g. in the decoder unit). From here This performs a deep copy of the data into the new vector
 * if users supply some l-value, in case they expect the vector to still be in a usable state after
 * the constructor call*/
AudioStream::AudioStream(std::vector<float> &stream, long channels = 0, long rate = 0)
    : stream_(stream), channels_(channels), rate_(rate)
{
}

/* A version of the constructor that is used when a user explicitly moves a vector as part of the
 * constructor (as the decoder unit does).*/
AudioStream::AudioStream(std::vector<float> &&stream, long channels = 0, long rate = 0)
    : stream_(std::move(stream)), channels_(channels), rate_(rate)
{
}

AudioStream::AudioStream() : stream_(), channels_(0), rate_(0) {}

} // namespace Audio
} // namespace fvp
