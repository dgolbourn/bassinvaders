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
  int Read(uint8_t* data, int size);

  uint8_t** data_;
  int size_;
  uint8_t* current_ptr_;
};

SamplesImpl::SamplesImpl(int samples)
{
  if(av_samples_alloc_array_and_samples(&data_, nullptr, av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT), samples, FFMPEG_FORMAT, 0) < 0)
  {
    throw Exception();
  }
  size_ = samples * av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT) * av_get_bytes_per_sample(FFMPEG_FORMAT);
  current_ptr_ = data_[0];
}

SamplesImpl::~SamplesImpl(void)
{
  av_freep(&data_[0]);
  av_freep(&data_);
}

int SamplesImpl::Read(uint8_t* data, int size)
{
  if(size > size_)
  {
    size = size_;
  }
  if(size > 0)
  {
    memcpy(data, current_ptr_, size);
    size_ -= size;
    current_ptr_ += size;
  }
  return size;
}

Samples::Samples(int samples)
{
  impl_ = std::make_shared<SamplesImpl>(samples);
}

uint8_t** Samples::Data(void)
{
  return impl_->data_;
}

void Samples::Size(int size) const
{
  impl_->size_ = size;
}

int Samples::Read(uint8_t* data, int size)
{
  return impl_->Read(data, size);
}

Samples::operator bool(void) const
{
  return impl_->size_ > 0;
}
}