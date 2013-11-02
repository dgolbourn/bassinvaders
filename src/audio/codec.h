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
  Codec(void);
  Codec(Format const& format);
  Codec(Codec const& other);
  Codec(Codec&& other);
  Codec& operator=(Codec other);
  ~Codec(void);

  AVCodecContext* operator->(void) const;
  AVCodecContext* Get(void) const; 
private:
  std::shared_ptr<AVCodecContext> codec_;
};

}

#endif