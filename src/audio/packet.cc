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

static AVPacket* InitAVPacket(void)
{
  AVPacket* packet = (AVPacket*)malloc(sizeof(AVPacket));
  if(!packet)
  {
    throw cstd::Exception();
  }
  av_init_packet(packet);
  return packet;
}

Packet::Packet(void) : packet_(InitAVPacket(), Deleter())
{
}

Packet::Packet(Packet const& other) : packet_(other.packet_)
{
}

Packet::Packet(Packet&& other) : packet_(std::move(other.packet_))
{
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