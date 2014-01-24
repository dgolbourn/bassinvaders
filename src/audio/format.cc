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
}

FormatImpl::~FormatImpl(void)
{
  avformat_close_input(&format_);
}

Format::Format(std::string const& filename)
{
  impl_ = std::make_shared<FormatImpl>(filename);
}

Format::operator AVFormatContext*(void) const
{
  return impl_->format_;
}

AVFormatContext* Format::operator->(void) const
{
  return impl_->format_;
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