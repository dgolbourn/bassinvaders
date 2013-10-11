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
  Frame(const Frame& other);
  Frame(Frame&& other);
  Frame& operator=(Frame other);
  ~Frame(void);

  AVFrame* operator->(void);
  AVFrame* get(void);
private:
  std::shared_ptr<AVFrame> frame_;
};

}

#endif