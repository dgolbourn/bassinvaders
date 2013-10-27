#include "samples.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{

class SamplesImpl
{
public:
  uint8_t** data_;
  int size_;

  SamplesImpl(uint8_t** data, int size);
  ~SamplesImpl(void);
};

SamplesImpl::SamplesImpl(uint8_t** data, int size)
{
  data_ = data;
  size_ = size;
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

Samples::Samples(void)
{
}

Samples::Samples(Samples const& other) : impl_(other.impl_)
{
}

Samples::Samples(Samples&& other) : impl_(std::move(other.impl_))
{
}

Samples& Samples::operator=(Samples other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Samples::~Samples(void)
{
}

uint8_t**& Samples::data(void)
{
  return impl_->data_;
}

int Samples::size(void)
{
  return impl_->size_;
}

}