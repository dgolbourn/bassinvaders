#include "decoder.h"
#include "ffmpeg_library.h"
#include "buffer.h"
#include "packet.h"
#include "format.h"
#include "codec.h"
#include "resampler.h"
#include "frame.h"
#include "ffmpeg_exception.h"
#include "filter.h"

namespace ffmpeg
{
class DecoderImpl
{
public:
  DecoderImpl(std::string const& filename);
  void Decode(void);
  void Silence(void);
  void Read(uint8_t* buffer, int size);
  Library const ffmpeg_;
  Format format_;
  Codec codec_;
  Resampler resampler_;
  Frame in_frame_;
  Frame out_frame_;
  Filter filter_;
  Buffer buffer_;
  bool decode_complete_;
  bool empty_;
};

DecoderImpl::DecoderImpl(std::string const& filename) : decode_complete_(false), empty_(false)
{
  format_ = Format(filename);
  codec_ = Codec(format_);
  resampler_ = Resampler(codec_);
  filter_ = Filter("volume=0.5", format_, codec_);
}

static Packet ReadAudio(Format const& format)
{
  bool audio_read = false;
  Packet packet;
  if(av_read_frame(format, packet) == 0)
  {
    if(packet->stream_index == format.AudioStream()->index)
    {
      audio_read = true;
    }
  }
  if(!audio_read)
  {
    packet = Packet();
  }
  return packet;
}

static bool DecodeAudio(Codec const& codec, Frame const& frame, Packet& packet)
{
  int frame_finished;
  int amount = avcodec_decode_audio4(codec, frame, &frame_finished, packet);
  if(amount < 0)
  {
    throw Exception();
  }
  packet += amount;
  return frame_finished != 0;
}

void DecoderImpl::Decode(void)
{
  if(Packet packet = ReadAudio(format_))
  {
    while(packet)
    {
      if(DecodeAudio(codec_, in_frame_, packet))
      {
        filter_.Add(in_frame_);
        while (filter_.Read(out_frame_))
        {
          buffer_.Add(resampler_(codec_, out_frame_));
          out_frame_.Clear();
        }
        in_frame_.Clear();
      }
    }
  }
  else
  {
    decode_complete_ = true;
  }
}

void DecoderImpl::Silence(void)
{
  static const int samples = 1024;
  buffer_.Add(Samples(samples));
}

void DecoderImpl::Read(uint8_t* buffer, int size)
{
  while(size)
  {
    if(buffer_)
    {
      int read_size = buffer_.Read(buffer, size);
      buffer += read_size;
      size -= read_size;
    }
    else
    {
      if(decode_complete_)
      {
        Silence();
        empty_ = true;
      }
      else
      {
        Decode();
      }
    }
  }
}

Decoder::Decoder(std::string const& filename)
{
  impl_ = std::make_shared<DecoderImpl>(filename);
}

void Decoder::Read(uint8_t* buffer, int size)
{
  impl_->Read(buffer, size);
}

Decoder::operator bool(void) const
{
  return !impl_->empty_;
}
}