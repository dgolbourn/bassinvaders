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

static AVFrame* InitAVFrame(void)
{
  AVFrame* frame = avcodec_alloc_frame();
  if(!frame)
  {
    throw Exception();
  }
  return frame;
}

Frame::Frame(void) : frame_(InitAVFrame(), Deleter())
{
}

Frame::Frame(Frame const& other) : frame_(other.frame_)
{
}

Frame::Frame(Frame&& other) : frame_(std::move(other.frame_))
{
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

AVFrame* Frame::Get(void)
{
  return frame_.get();
}

}