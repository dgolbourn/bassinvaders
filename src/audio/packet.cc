#include "packet.h"

namespace ffmpeg
{
Packet::Packet(void) : packet_(new AVPacket, av_free_packet)
{
  av_init_packet(packet_.get());
}

AVPacket* Packet::operator->(void) const
{
  return packet_.operator->();
}

Packet::operator AVPacket*(void) const
{
  return packet_.get();
}

void Packet::Next(int amount_used)
{
  packet_->data += amount_used;
  packet_->size -= amount_used;
}

Packet::operator bool(void) const
{
  return packet_->size > 0;
}
}