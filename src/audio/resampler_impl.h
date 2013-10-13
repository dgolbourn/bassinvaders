#ifndef RESAMPLER_IMPL_H_
#define RESAMPLER_IMPL_H_

extern "C"
{
#include <libswresample/swresample.h>
}

#include "samples.h"
#include "codec.h"

namespace ffmpeg
{

class ResamplerImpl
{
public:
  SwrContext* swr_;
  int input_sample_rate_;
  int output_channels_;
  int output_sample_rate_;
  AVSampleFormat output_format_;
  uint64_t output_channel_layout_;

  ResamplerImpl(Codec& codec);
  ~ResamplerImpl(void);
  Samples Resample(uint8_t const** input, int in_samples);
};

}

#endif