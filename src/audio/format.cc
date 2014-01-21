#include "format.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{
class FormatImpl
{
public:
  FormatImpl(std::string const& filename);
  void Destroy(void);

  ~FormatImpl(void);

  AVFormatContext* format_;
  AVStream* audio_stream_;
};

FormatImpl::FormatImpl(std::string const& filename)
{
  format_ = avformat_alloc_context();
  if(!format_)
  {
    throw Exception();
  }
  if(avformat_open_input(&format_, filename.c_str(), nullptr, nullptr))
  {
    avformat_free_context(format_);
    throw Exception();
  }

  try
  {
    if(avformat_find_stream_info(format_, nullptr) < 0)
    {
      throw Exception();
    }
    audio_stream_ = nullptr;
    for(unsigned int i = 0; i < format_->nb_streams; ++i)
    {
      if(AVMEDIA_TYPE_AUDIO == format_->streams[i]->codec->codec_type)
      {
        audio_stream_ = format_->streams[i];
        break;
      }
    }
    if(!audio_stream_)
    {
      throw Exception();
    }
  }
  catch(...)
  {
    Destroy();
    throw;
  }
}

void FormatImpl::Destroy(void)
{
  avformat_close_input(&format_);
  avformat_free_context(format_);
}

FormatImpl::~FormatImpl(void)
{
  Destroy();
}

Format::Format(std::string const& filename)
{
  impl_ = std::make_shared<FormatImpl>(filename);
}

Format::operator AVFormatContext*(void) const
{
  return impl_->format_;
}

AVStream* Format::AudioStream(void) const
{
  return impl_->audio_stream_;
}

bool Format::Read(Packet& packet)
{
  bool packet_read = false;
  if(av_read_frame(impl_->format_, packet) == 0)
  {
    packet_read = true;
  }
  return packet_read;
}
}