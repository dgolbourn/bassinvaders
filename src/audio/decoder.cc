#include "decoder.h"
#include "ffmpeg_library.h"
#include "buffer.h"
#include "packet.h"
#include "format.h"
#include "codec.h"
#include "resampler.h"
#include "frame.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{
class DecoderImpl
{
public:
  DecoderImpl(std::string const& filename);
  void Decode(void);
  bool Empty(void) const;
  void Read(uint8_t* buffer, int size);
  ffmpeg::Library const ffmpeg_;
  ffmpeg::Format format_;
  ffmpeg::Codec codec_;
  ffmpeg::Resampler resampler_;
  ffmpeg::Frame frame_;
  ffmpeg::Buffer buffer_;
  bool finished_;
};

DecoderImpl::DecoderImpl(std::string const& filename) : finished_(false)
{
  format_ = ffmpeg::Format(filename);
  codec_ = ffmpeg::Codec(format_);
  resampler_ = ffmpeg::Resampler(codec_);
}

static Packet ReadAudio(Format const& format)
{
  bool audio_read = false;
  Packet packet;
  if(av_read_frame(format.format(), packet) == 0)
  {
    if(packet->stream_index == format.audio_stream()->index)
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
    throw ffmpeg::Exception();
  }
  packet += amount;
  return frame_finished != 0;
}

void DecoderImpl::Decode(void)
{
  if(ffmpeg::Packet packet = ReadAudio(format_))
  {
    while(packet)
    {
      if(DecodeAudio(codec_, frame_, packet))
      {
        buffer_.Add(resampler_(frame_));
        frame_.Clear();
      }
    }
  }
  else
  {
    finished_ = true;
  }
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
      if(finished_)
      {
        memset(buffer, 0, size);
        size = 0;
      }
      else
      {
        Decode();
      }
    }
  }
}

bool DecoderImpl::Empty(void) const
{
  return !buffer_ && finished_;
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
  return !impl_->Empty();
}
}