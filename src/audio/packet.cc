#include "packet.h"

namespace ffmpeg
{
Packet::Packet(void)
{
  packet_ = std::make_shared<AVPacket>();
  av_init_packet(packet_.get());
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
  av_free_packet(packet_.get());
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