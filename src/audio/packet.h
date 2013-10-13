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
  Packet(const Packet& other);
  Packet(Packet&& other);
  Packet& operator=(Packet other);
  ~Packet(void);

  AVPacket* operator->(void);
  AVPacket* Get(void);
private:
  std::shared_ptr<AVPacket> packet_;
};

}

#endif