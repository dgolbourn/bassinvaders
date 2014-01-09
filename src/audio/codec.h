#ifndef CODEC_H_
#define CODEC_H_

extern "C"
{
#include "libavcodec/avcodec.h"
}
#include <memory>
#include "format.h"

namespace ffmpeg
{
class Codec
{
public:
  Codec(void) = default;
  Codec(Format const& format);
  AVCodecContext* operator->(void) const;
  operator AVCodecContext*(void) const;
private:
  std::shared_ptr<AVCodecContext> codec_;
};
}
#endif