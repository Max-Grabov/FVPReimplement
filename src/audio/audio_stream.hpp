#pragma once

#include <vector>

namespace fvp
{

namespace Audio
{
class AudioStream
{
private:
  std::vector<float> stream_;
  long channels_;
  long rate_;

public:
  AudioStream(std::vector<float> &, long, long);
  AudioStream(std::vector<float> &&, long, long);
  AudioStream();
  inline const std::vector<float> &GetStream() const { return stream_; }
  inline long GetChannels() const { return channels_; }
  inline long GetRate() const { return rate_; }
};
} // namespace Audio
} // namespace fvp
