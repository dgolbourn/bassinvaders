#include "decoder.h"
#include "frame.h"
#include "volume.h"
#include "graph.h"
#include "sink.h"
#include "source.h"
#include "ffmpeg_exception.h"
namespace ffmpeg
{
class DecoderImpl
{
public:
  DecoderImpl(std::string const& filename);
  int Read(uint8_t* buffer, int size);
  Frame frame_;
  Graph graph_;
  Sink sink_;
  Source source_;
  Volume volume_;
};

static void Link(AVFilterContext* out, AVFilterContext* in)
{
  if(avfilter_link(out, 0, in, 0))
  {
    throw Exception();
  }
}

DecoderImpl::DecoderImpl(std::string const& filename)
{
  avfilter_register_all();
  source_ = Source(filename, graph_);
  sink_ = Sink(graph_);
  volume_ = Volume(graph_);
  Link(source_, volume_);
  Link(volume_, sink_);
  graph_();
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
    else if(sink_(frame_))
    {
    } 
    else
    {
      break;
    }
  }
  return start_size - size;
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
  impl_->volume_(volume);
}
}