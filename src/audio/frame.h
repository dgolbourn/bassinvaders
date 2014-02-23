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
  operator AVFrame*(void) const;
  void Seek(void);
  int Read(uint8_t* buffer, int size);
  void Close(void);
  explicit operator bool(void) const;
private:
  std::shared_ptr<class FrameImpl> impl_;
};
}
#endif