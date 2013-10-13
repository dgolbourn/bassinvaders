#include "packet.h"
#include "ffmpeg_exception.h"
#include "cstd_exception.h"

namespace ffmpeg
{

class Deleter
{
public:
  void operator()(AVPacket* packet)
  {
    av_free_packet(packet);
    free(packet);
  }
};

Packet::Packet(void)
{
  AVPacket* packet = (AVPacket*)malloc(sizeof(AVPacket));
  if(!packet)
  {
    throw cstd::Exception();
  }
  packet_ = std::shared_ptr<AVPacket>(packet, Deleter());
  av_init_packet(packet);
}

Packet::Packet(const Packet& other)
{
  packet_ = other.packet_;
}

Packet::Packet(Packet&& other)
{
  packet_ = other.packet_;
  other.packet_.reset();
}

Packet& Packet::operator=(Packet other)
{
  std::swap(packet_, other.packet_);
  return *this;
}

Packet::~Packet(void)
{
}

AVPacket* Packet::operator->(void)
{
  return packet_.operator->();
}

AVPacket* Packet::Get(void)
{
  return packet_.get();
}

}