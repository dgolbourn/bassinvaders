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
    avcodec_free_frame(&frame);
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

AVFrame* Frame::operator->(void) const
{
  return frame_.operator->();
}

Frame::operator AVFrame* (void) const
{
  return frame_.get();
}

void Frame::Clear(void)
{
  av_frame_unref(frame_.get());
}

uint8_t const** Frame::Data(void) const
{
  return (uint8_t const**)frame_.get()->data;
}
}