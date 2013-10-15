extern "C"
{
#include <libavutil/opt.h>
#include "libswresample/swresample.h"
}
#include "resampler.h"
#include "ffmpeg_exception.h"
#include "audio_format.h"

namespace ffmpeg
{

class ResamplerImpl
{
public:
  SwrContext* swr_;
  int input_sample_rate_;

  ResamplerImpl(Codec& codec);
  ~ResamplerImpl(void);
  Samples Resample(uint8_t const** input, int in_samples);
};

ResamplerImpl::ResamplerImpl(Codec& codec)
{
  swr_ = swr_alloc();
  if(!swr_)
  {
    throw Exception();
  }

  av_opt_set_int(swr_, "in_channel_layout", codec->channel_layout, 0);
  av_opt_set_int(swr_, "out_channel_layout", FFMPEG_CHANNEL_LAYOUT, 0);
  av_opt_set_int(swr_, "in_sample_rate", codec->sample_rate, 0);
  av_opt_set_int(swr_, "out_sample_rate", SAMPLE_RATE, 0);
  av_opt_set_sample_fmt(swr_, "in_sample_fmt", codec->sample_fmt, 0);
  av_opt_set_sample_fmt(swr_, "out_sample_fmt", FFMPEG_FORMAT, 0);
    
  if(swr_init(swr_))
  {
    throw Exception();
  }

  input_sample_rate_ = codec->sample_rate;
}

ResamplerImpl::~ResamplerImpl(void)
{
  swr_free(&swr_);
}

Samples ResamplerImpl::Resample(uint8_t const** input, int in_samples)
{
  uint8_t** output;
  int64_t delay = swr_get_delay(swr_, input_sample_rate_) + in_samples;
  int out_samples = (int)av_rescale_rnd(delay, SAMPLE_RATE, input_sample_rate_, AV_ROUND_UP);    
  int linesize;
  if(av_samples_alloc_array_and_samples(&output, &linesize, CHANNELS, out_samples, FFMPEG_FORMAT, 1) < 0)
  {
    throw Exception();
  }
  int conv_samples = swr_convert(swr_, output, out_samples, input, in_samples);
  if(conv_samples < 0)
  {
    throw Exception();
  }
  int size = av_samples_get_buffer_size(&linesize, CHANNELS, conv_samples, FFMPEG_FORMAT, 1);
  return Samples(output, size);
}

Resampler::Resampler(void)
{
}

Resampler::Resampler(Codec& codec)
{
  impl_ = std::shared_ptr<ResamplerImpl>(new ResamplerImpl(codec));
}
  
Resampler::Resampler(const Resampler& other)
{
  impl_ = other.impl_;
}

Resampler::Resampler(Resampler&& other)
{
  impl_ = other.impl_;
  other.impl_.reset();
}

Resampler& Resampler::operator=(Resampler other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Resampler::~Resampler(void)
{
}

Samples Resampler::Resample(uint8_t const** input, int in_samples)
{
  return impl_->Resample(input, in_samples);
}

}
