extern "C"
{
#include "libavutil/opt.h"
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
  ResamplerImpl(Codec const& codec);
  Samples Resample(Frame const& frame);
  void Destroy(void);

  ~ResamplerImpl(void);

  SwrContext* swr_;
  int input_sample_rate_;
  int channels_;
};

void ResamplerImpl::Destroy(void)
{
  swr_free(&swr_);
}

ResamplerImpl::ResamplerImpl(Codec const& codec)
{
  swr_ = swr_alloc();
  if(!swr_)
  {
    throw Exception();
  }

  try
  {
    int err = 0;
    err |= av_opt_set_int(swr_, "in_channel_layout", codec->channel_layout, 0);
    err |= av_opt_set_int(swr_, "out_channel_layout", FFMPEG_CHANNEL_LAYOUT, 0);
    err |= av_opt_set_int(swr_, "in_sample_rate", codec->sample_rate, 0);
    err |= av_opt_set_int(swr_, "out_sample_rate", FFMPEG_SAMPLE_RATE, 0);
    err |= av_opt_set_sample_fmt(swr_, "in_sample_fmt", codec->sample_fmt, 0);
    err |= av_opt_set_sample_fmt(swr_, "out_sample_fmt", FFMPEG_FORMAT, 0);
    if(err)
    {
      throw Exception();
    }

    if(swr_init(swr_))
    {
      throw Exception();
    }

    input_sample_rate_ = codec->sample_rate;
    channels_ = av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT);
  }
  catch(...)
  {
    Destroy();
    throw;
  }
}

ResamplerImpl::~ResamplerImpl(void)
{
  Destroy();
}

Samples ResamplerImpl::Resample(Frame const& frame)
{
  int in_samples = frame->nb_samples;
  int64_t delay = swr_get_delay(swr_, input_sample_rate_) + in_samples;
  int out_samples = static_cast<int>(av_rescale_rnd(delay, FFMPEG_SAMPLE_RATE, input_sample_rate_, AV_ROUND_UP));   
  Samples samples(out_samples);
  int conv_samples = swr_convert(swr_, samples.Data(), out_samples, frame.Data(), in_samples);
  if(conv_samples < 0)
  {
    throw Exception();
  }
  samples.Size(av_samples_get_buffer_size(nullptr, channels_, conv_samples, FFMPEG_FORMAT, 0));
  return samples;
}

Resampler::Resampler(Codec const& codec)
{
  impl_ = std::make_shared<ResamplerImpl>(codec);
}

Samples Resampler::operator()(Frame const& frame)
{
  return impl_->Resample(frame);
}
}
