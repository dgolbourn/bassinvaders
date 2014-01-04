#include "ffmpeg_library.h"

extern "C"
{
#include "libavformat/avformat.h"
}

namespace ffmpeg
{ 
static bool registered;

Library::Library(void)
{
  if(!registered)
  {
    av_register_all();
    registered = true;
  }
}
}
