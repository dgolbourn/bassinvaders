#include "samples.h"
extern "C"
{
#include "libavutil/samplefmt.h"
#include "libavutil/channel_layout.h"
}
#include "ffmpeg_exception.h"
#include "audio_format.h"
#include "cstd_exception.h"

namespace ffmpeg
{
class SamplesImpl
{
public:
  SamplesImpl(Frame const& frame);
  int Read(uint8_t* data, int size);

  ~SamplesImpl(void);

  uint8_t** data_;
  int size_;
  uint8_t* current_ptr_;
};

SamplesImpl::SamplesImpl(Frame const& frame)
{
  if(av_samples_alloc_array_and_samples(&data_, nullptr, av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT), frame->nb_samples, FFMPEG_FORMAT, 0) < 0)
  {
    throw Exception();
  }
  size_ = frame->nb_samples * av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT) * av_get_bytes_per_sample(FFMPEG_FORMAT);
  if(!memcpy(data_[0], frame.Data()[0], size_))
  {
    throw cstd::Exception();
  }
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

Samples::Samples(Frame const& frame)
{
  impl_ = std::make_shared<SamplesImpl>(frame);
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