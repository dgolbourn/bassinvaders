#include "decoder.h"
#include "ffmpeg_library.h"
#include "packet.h"
#include "format.h"
#include "codec.h"
#include "frame.h"
#include "ffmpeg_exception.h"
#include "filter.h"
#include "cstd_exception.h"

namespace ffmpeg
{
class DecoderImpl
{
public:
  DecoderImpl(std::string const& filename);
  void Read(uint8_t* buffer, int size);
  void Volume(double volume);
  Library const ffmpeg_;
  Format format_;
  Codec codec_;
  Filter filter_;
  Packet packet_;
  Frame frame_;
  bool empty_;
};

DecoderImpl::DecoderImpl(std::string const& filename)
{
  empty_ = false;
  format_ = Format(filename);
  codec_ = Codec(format_);
  filter_ = Filter(format_, codec_);
}

void DecoderImpl::Read(uint8_t* buffer, int size)
{
  while(size)
  {
    if(frame_)
    {
      if(int read_size = frame_.Read(buffer, size))
      {
        size -= read_size;
        buffer += read_size;
      }
      else
      {
        frame_.Close();
      }
    }
    else if(filter_.Read(frame_))
    {
    } 
    else if(packet_)
    {
      while(packet_)
      {
        if(packet_.Read(codec_, frame_))
        {
          filter_.Add(frame_);
          frame_.Close();
        }
        else
        {
          break;
        }
      }
      packet_.Close();
    }
    else if(format_.Read(packet_))
    {
    }
    else if(codec_.Read(frame_))
    {
    }
    else
    {
      if(!memset(buffer, 0, size))
      {
        throw cstd::Exception();
      }
      size = 0;
      empty_ = true;
    }
  }
}

void DecoderImpl::Volume(double volume)
{
  filter_.Volume(volume);
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

void Decoder::Volume(double volume)
{
  impl_->Volume(volume);
}
}