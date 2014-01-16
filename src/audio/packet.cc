#include "packet.h"

namespace ffmpeg
{
void FreeAVPacket(AVPacket* packet)
{
  av_free_packet(packet);
  delete packet;
}

Packet::Packet(void) : packet_(new AVPacket, FreeAVPacket)
{
  av_init_packet(packet_.get());
  packet_->data = nullptr;
  packet_->size = 0;
}

AVPacket* Packet::operator->(void) const
{
  return packet_.operator->();
}

Packet::operator AVPacket*(void) const
{
  return packet_.get();
}

void Packet::operator+=(int amount_used)
{
  packet_->data += amount_used;
  packet_->size -= amount_used;
}

Packet::operator bool(void) const
{
  return packet_->size > 0;
}
}