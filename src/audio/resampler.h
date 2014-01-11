#ifndef RESAMPLER_H_
#define RESAMPLER_H_

#include <memory>

#include "codec.h"
#include "samples.h"

namespace ffmpeg
{
class Resampler
{
public:
  Resampler(void) = default;
  Resampler(Codec const& codec);
  Samples operator()(uint8_t const** input, int in_samples);
private:
  std::shared_ptr<class ResamplerImpl> impl_;
};
}

#endif