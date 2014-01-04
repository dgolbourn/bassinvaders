#include "codec.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{
static AVCodecContext* InitAVCodecContext(Format const& format)
{
  AVCodecContext* codec = format.audio_stream()->codec;
  codec->codec = avcodec_find_decoder(codec->codec_id);
  if(!codec->codec)
  {
    throw Exception();
  }
  if(avcodec_open2(codec, codec->codec, nullptr))
  {
    throw Exception();
  }
  return codec;
}

Codec::Codec(Format const& format) : codec_(InitAVCodecContext(format), &avcodec_close)
{
}

Codec::Codec(void)
{    
}

AVCodecContext* Codec::operator->(void) const
{
  return codec_.operator->();
}

Codec::operator AVCodecContext*(void) const
{
  return codec_.get();
}
}