#include "format.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{
class FormatImpl
{
public:
  FormatImpl(std::string const& filename);

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
    throw Exception();
  }
  if(avformat_find_stream_info(format_, nullptr) < 0)
  {
    throw Exception();
  }

  audio_stream_ = nullptr;
  for(unsigned int i = 0; i < format_->nb_streams; ++i)
  {
    if(format_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
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

FormatImpl::~FormatImpl(void)
{
  if(format_)
  {
    avformat_close_input(&format_);
    avformat_free_context(format_);
  }
}

Format::Format(void)
{    
}

Format::Format(std::string const& filename) : impl_(new FormatImpl(filename))
{
}

Format::Format(Format const& other) : impl_(other.impl_)
{
}

Format::Format(Format&& other) : impl_(std::move(other.impl_))
{
}

Format& Format::operator=(Format other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Format::~Format(void)
{
}

AVFormatContext* Format::format(void) const
{
  return impl_->format_;
}

AVStream* Format::audio_stream(void) const
{
  return impl_->audio_stream_;
}
}