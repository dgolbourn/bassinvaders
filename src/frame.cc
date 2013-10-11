#include "frame.h"

extern "C"
{
#include "libavcodec/avcodec.h"
}
#include "ffmpeg_exception.h"

namespace ffmpeg
{

class Deleter
{
public:
  void operator()(AVFrame* frame)
  {
    av_free(frame);
  }
};

Frame::Frame(void)
{
  AVFrame* frame = avcodec_alloc_frame();
  if(!frame)
  {
    throw ffmpeg::Exception();
  }
  frame_ = std::shared_ptr<AVFrame>(frame, Deleter());
}

Frame::Frame(const Frame& other)
{
  frame_ = other.frame_;
}

Frame::Frame(Frame&& other)
{
  frame_ = other.frame_;
  other.frame_.reset();
}

Frame& Frame::operator=(Frame other)
{
  std::swap(frame_, other.frame_);
  return *this;
}

Frame::~Frame(void)
{
}

AVFrame* Frame::operator->(void)
{
  return frame_.operator->();
}

AVFrame* Frame::get(void)
{
  return frame_.get();
}

}