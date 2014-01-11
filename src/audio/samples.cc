#include "samples.h"
extern "C"
{
#include "libavutil/samplefmt.h"
#include "libavutil/channel_layout.h"
}
#include "ffmpeg_exception.h"
#include "audio_format.h"

namespace ffmpeg
{
class SamplesImpl
{
public:
  SamplesImpl(int samples);
  ~SamplesImpl(void);

  uint8_t** data_;
  int size_;
};

SamplesImpl::SamplesImpl(int samples)
{
  if(av_samples_alloc_array_and_samples(&data_, nullptr, av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT), samples, FFMPEG_FORMAT, 0) < 0)
  {
    throw Exception();
  }
  size_ = 0;
}

SamplesImpl::~SamplesImpl(void)
{
  av_freep(&data_[0]);
  av_freep(data_);
}

Samples::Samples(int samples)
{
  impl_ = std::make_shared<SamplesImpl>(samples);
}

uint8_t** Samples::data(void)
{
  return impl_->data_;
}

int Samples::size(void) const
{
  return impl_->size_;
}

void Samples::size(int size) const
{
  impl_->size_ = size;
}
}