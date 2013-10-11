#include "samples.h"
#include "samples_impl.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{

SamplesImpl::SamplesImpl(uint8_t** data, int size)
{
  data_ = data;
  size_ = size;
}

SamplesImpl::SamplesImpl(int channels, int size, AVSampleFormat format)
{
  if(av_samples_alloc_array_and_samples(&data_, nullptr, channels, size, format, 0) < 0)
  {
    data_ = nullptr;
    throw ffmpeg::Exception();
  }
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

Samples::Samples(uint8_t** data, int size)
{
  impl_ = std::shared_ptr<SamplesImpl>(new SamplesImpl(data, size));
}

Samples::Samples(void)
{
}

Samples::Samples(int channels, int size, AVSampleFormat format)
{
  impl_ = std::shared_ptr<SamplesImpl>(new SamplesImpl(channels, size, format));
}

Samples::Samples(const Samples& other)
{
  impl_ = other.impl_;
}

Samples::Samples(Samples&& other)
{
  impl_ = other.impl_;
  other.impl_.reset();
}

Samples& Samples::operator=(Samples other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Samples::~Samples(void)
{
}

uint8_t** Samples::data(void)
{
  return impl_->data_;
}

int Samples::size(void)
{
  return impl_->size_;
}

}