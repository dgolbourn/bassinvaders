#include "decoder.h"
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
  Filter filter_;
  Frame frame_;
};

DecoderImpl::DecoderImpl(std::string const& filename)
{
  filter_ = Filter(filename);
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