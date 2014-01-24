#ifndef PACKET_H_
#define PACKET_H_

extern "C"
{
#include "libavcodec/avcodec.h"
}
#include <memory>
#include "frame.h"

namespace ffmpeg
{
class Packet
{
public:
  Packet(void);
  AVPacket* operator->(void) const;
  operator AVPacket*(void) const;
  explicit operator bool(void) const;
  void Close(void);
  bool Read(class Codec const& codec, Frame const& frame);
private:
  std::shared_ptr<AVPacket> packet_;
};
}
#endif