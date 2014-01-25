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

Packet::Packet(void) : impl_(new AVPacket, FreeAVPacket)
{
  av_init_packet(impl_.get());
  impl_->data = nullptr;
  impl_->size = 0;
}

AVPacket* Packet::operator->(void) const
{
  return impl_.operator->();
}

Packet::operator AVPacket*(void) const
{
  return impl_.get();
}

Packet::operator bool(void) const
{
  return impl_->size > 0;
}

void Packet::Close(void)
{
  av_packet_unref(impl_.get());
}

bool Packet::Read(Codec const& codec, Frame const& frame)
{
  int frame_read = 0;
  if(impl_->stream_index == codec.Stream())
  {
    while(!frame_read)
    {
      int amount = avcodec_decode_audio4(codec, frame, &frame_read, impl_.get());
      if(amount < 0)
      {
        throw Exception();
      }
      impl_->data += amount;
      impl_->size -= amount;
    }
  }
  return frame_read != 0;
}
}