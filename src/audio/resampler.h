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
  Resampler(void);
  Resampler(Codec const& codec);
  Resampler(Resampler const& other);
  Resampler(Resampler&& other);
  Resampler& operator=(Resampler other);
  ~Resampler(void);

  Samples Resample(uint8_t const** input, int in_samples);
private:
  std::shared_ptr<class ResamplerImpl> impl_;
};

}

#endif