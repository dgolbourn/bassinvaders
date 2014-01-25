#include "format.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{
static AVFormatContext* InitAVFormat(std::string const& filename)
{
  AVFormatContext* format = nullptr;
  if(avformat_open_input(&format, filename.c_str(), nullptr, nullptr))
  {
    throw Exception();
  }
  return format;
}

static void FreeAVFormat(AVFormatContext* format)
{
  avformat_close_input(&format);
}

Format::Format(std::string const& filename) : impl_(InitAVFormat(filename), FreeAVFormat)
{
}

Format::operator AVFormatContext*(void) const
{
  return impl_.get();
}

AVFormatContext* Format::operator->(void) const
{
  return impl_.get();
}

bool Format::Read(Packet& packet)
{
  bool packet_read = false;
  if(av_read_frame(impl_.get(), packet) == 0)
  {
    packet_read = true;
  }
  return packet_read;
}
}