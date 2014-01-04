#ifndef PACKET_H_
#define PACKET_H_

extern "C"
{
#include "libavcodec/avcodec.h"
}
#include <memory>

namespace ffmpeg
{
class Packet
{
public:
  Packet(void);
  AVPacket* operator->(void) const;
  operator AVPacket*(void) const;
  void Next(int amount_used);
  explicit operator bool(void) const;
private:
  std::shared_ptr<AVPacket> packet_;
};
}
#endif