#include "decoder.h"
#include "ffmpeg_library.h"
#include "packet.h"
#include "format.h"
#include "codec.h"
#include "frame.h"
#include "ffmpeg_exception.h"
#include "filter.h"

namespace ffmpeg
{
class DecoderImpl
{
public:
  DecoderImpl(std::string const& filename);
  int Read(uint8_t* buffer, int size);
  void Volume(double volume);
  Library const ffmpeg_;
  Format format_;
  Codec codec_;
  Filter filter_;
  Packet packet_;
  Frame frame_;
};

DecoderImpl::DecoderImpl(std::string const& filename)
{
  format_ = Format(filename);
  codec_ = Codec(format_);
  filter_ = Filter(format_, codec_);
}

int DecoderImpl::Read(uint8_t* buffer, int size)
{
  int start_size = size;
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
      break;
    }
  }
  return start_size - size;
}

void DecoderImpl::Volume(double volume)
{
  filter_.Volume(volume);
}

Decoder::Decoder(std::string const& filename)
{
  impl_ = std::make_shared<DecoderImpl>(filename);
}

int Decoder::Read(uint8_t* buffer, int size)
{
  return impl_->Read(buffer, size);
}

void Decoder::Volume(double volume)
{
  impl_->Volume(volume);
}
}