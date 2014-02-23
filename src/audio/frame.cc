#include "frame.h"
extern "C"
{
#include "libavcodec/avcodec.h"
}
#include "ffmpeg_exception.h"
#include "cstd_exception.h"
#include "audio_format.h"
namespace ffmpeg
{
class FrameImpl
{
public:
  FrameImpl(void);
  void Seek(void);
  int Read(uint8_t* buffer, int size);
  void Close(void);

  ~FrameImpl(void);

  uint8_t* buffer_ptr_;
  int buffer_size_;
  AVFrame* frame_;
};

FrameImpl::FrameImpl(void)
{
  buffer_ptr_ = nullptr;
  buffer_size_ = 0;
  frame_ = avcodec_alloc_frame();
  if(!frame_)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

FrameImpl::~FrameImpl(void)
{
  avcodec_free_frame(&frame_);
}

void FrameImpl::Seek(void)
{
  buffer_ptr_ = frame_->data[0];
  buffer_size_ = frame_->nb_samples * av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT) * av_get_bytes_per_sample(FFMPEG_FORMAT);
}

int FrameImpl::Read(uint8_t* buffer, int size)
{
  if(size > buffer_size_)
  {
    size = buffer_size_;
  }
  if(!memcpy(buffer, buffer_ptr_, size))
  {
    BOOST_THROW_EXCEPTION(cstd::Exception());
  }
  buffer_ptr_ += size;
  buffer_size_ -= size;
  return size;
}

void FrameImpl::Close(void)
{
  av_frame_unref(frame_);
  avcodec_get_frame_defaults(frame_);
}

Frame::Frame(void)
{
  impl_ = std::make_shared<FrameImpl>();
}

Frame::operator AVFrame*(void) const
{
  return impl_->frame_;
}

void Frame::Seek(void)
{
  impl_->Seek();
}

int Frame::Read(uint8_t* buffer, int size)
{
  return impl_->Read(buffer, size);
}

void Frame::Close(void)
{
  impl_->Close();
}

Frame::operator bool(void) const
{
  return impl_->frame_->nb_samples > 0;
}
}