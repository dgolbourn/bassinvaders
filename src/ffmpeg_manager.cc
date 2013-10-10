#include "ffmpeg_manager.h"

extern "C"
{
#include "libavformat/avformat.h"
}

namespace ffmpeg
{ 
static bool registered;

void init(void)
{
  if(!registered)
  {
    av_register_all();
    registered = true;
  }
}
}
