#ifndef FRAME_H_
#define FRAME_H_

extern "C"
{
#include "libavutil/frame.h"
}
#include <memory>

namespace ffmpeg
{
class Frame
{
public:
  Frame(void);
  AVFrame* operator->(void) const;
  operator AVFrame*(void) const;
  void Clear(void);
  uint8_t const** Data(void) const;
private:
  std::shared_ptr<AVFrame> frame_;
};
}
#endif