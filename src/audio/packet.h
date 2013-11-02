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
  Packet(Packet const& other);
  Packet(Packet&& other);
  Packet& operator=(Packet other);
  ~Packet(void);

  AVPacket* operator->(void) const;
  AVPacket* Get(void) const;
  void Next(int amount_used);
  bool Empty(void) const;
private:
  std::shared_ptr<AVPacket> packet_;
};

}

#endif