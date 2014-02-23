#include "ffmpeg_exception.h"
extern "C"
{
#include "libavutil/error.h"
}
namespace ffmpeg
{
std::string Error(int error)
{
  char errbuf[1024];
  (void)av_strerror(error, errbuf, 1024);
  return errbuf;
}
}