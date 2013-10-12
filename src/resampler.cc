extern "C"
{
#include <libavutil/opt.h>
}
#include "resampler.h"
#include "resampler_impl.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{

ResamplerImpl::ResamplerImpl(Codec& codec)
{
  swr_ = swr_alloc();
  if(!swr_)
  {
    throw ffmpeg::Exception();
  }

  output_sample_rate_ = 22050;
  output_format_ = AV_SAMPLE_FMT_S16;
  output_channel_layout_ = AV_CH_LAYOUT_STEREO;

  av_opt_set_int(swr_, "in_channel_layout", codec->channel_layout, 0);
  av_opt_set_int(swr_, "out_channel_layout", output_channel_layout_, 0);
  av_opt_set_int(swr_, "in_sample_rate", codec->sample_rate, 0);
  av_opt_set_int(swr_, "out_sample_rate", output_sample_rate_, 0);
  av_opt_set_sample_fmt(swr_, "in_sample_fmt", codec->sample_fmt, 0);
  av_opt_set_sample_fmt(swr_, "out_sample_fmt", output_format_, 0);
    
  if(swr_init(swr_))
  {
    throw ffmpeg::Exception();
  }

  input_sample_rate_ = codec->sample_rate;
  output_channels_ =  av_get_channel_layout_nb_channels(output_channel_layout_);
}

ResamplerImpl::~ResamplerImpl(void)
{
  if(swr_)
  {
    swr_free(&swr_);
  }
}

Samples ResamplerImpl::Resample(uint8_t const** input, int in_samples)
{
  uint8_t** output;
  int64_t delay = swr_get_delay(swr_, input_sample_rate_) + in_samples;
  int out_samples = (int)av_rescale_rnd(delay, output_sample_rate_, input_sample_rate_, AV_ROUND_UP);    
  int linesize;
  if(av_samples_alloc_array_and_samples(&output, &linesize, output_channels_, out_samples, output_format_, 1) < 0)
  {
    throw ffmpeg::Exception();
  }
  int conv_samples = swr_convert(swr_, output, out_samples, input, in_samples);
  if(conv_samples < 0)
  {
    throw ffmpeg::Exception();
  }
  int size = av_samples_get_buffer_size(&linesize, output_channels_, conv_samples, output_format_, 1);
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
