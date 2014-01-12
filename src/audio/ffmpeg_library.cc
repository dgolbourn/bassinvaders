#include "ffmpeg_library.h"

extern "C"
{
#include "libavformat/avformat.h"
}

namespace ffmpeg
{ 
Library::Library(void)
{
  av_register_all();
}
}
