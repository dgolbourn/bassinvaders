#include "samples.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{
class SamplesImpl
{
public:
  SamplesImpl(uint8_t** data, int size);

  ~SamplesImpl(void);

  uint8_t** data_;
  int size_;
};

SamplesImpl::SamplesImpl(uint8_t** data, int size) : data_(data), size_(size)
{
}

SamplesImpl::~SamplesImpl(void)
{
  if(data_)
  {
    av_freep(&data_[0]);
    av_freep(data_);
  }
}

Samples::Samples(uint8_t** data, int size) : impl_(new SamplesImpl(data, size))
{
}

Samples::Samples(void) noexcept
{
}

uint8_t**& Samples::data(void) const
{
  return impl_->data_;
}

int Samples::size(void) const
{
  return impl_->size_;
}
}