#include "packet.h"
#include "ffmpeg_exception.h"
#include "codec.h"

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

Packet::operator bool(void) const
{
  return packet_->size > 0;
}

void Packet::Close(void)
{
  av_packet_unref(packet_.get());
}

void Packet::Read(Codec const& codec, Frame const& frame)
{
  int frame_read = 0;
  while(!frame_read)
  {
    int amount = avcodec_decode_audio4(codec, frame, &frame_read, packet_.get());
    if(amount < 0)
    {
      throw Exception();
    }
    packet_->data += amount;
    packet_->size -= amount;
  }
}
}