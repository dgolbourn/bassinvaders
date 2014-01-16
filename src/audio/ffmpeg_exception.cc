#include "ffmpeg_exception.h"

namespace ffmpeg
{
static std::string ErrorString(int error)
{
  char errbuf[1024];
  (void)av_strerror(error, errbuf, 1024);
  return errbuf;
}

Exception::Exception(int error) : std::runtime_error(ErrorString(error))
{ 
}
}