#include "codec.h"
#include "ffmpeg_exception.h"

namespace ffmpeg
{

class Deleter
{
public:
  void operator()(AVCodecContext* codec)
  {
    avcodec_close(codec);
  }
};

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

Codec::Codec(Format const& format) : codec_(InitAVCodecContext(format), Deleter())
{
}

Codec::Codec(void)
{    
}

Codec::Codec(Codec const& other) : codec_(other.codec_)
{
}

Codec::Codec(Codec&& other) : codec_(std::move(other.codec_))
{
}

Codec& Codec::operator=(Codec other)
{
  std::swap(codec_, other.codec_);
  return *this;
}

Codec::~Codec(void)
{
}

AVCodecContext* Codec::operator->(void) const
{
  return codec_.operator->();
}

AVCodecContext* Codec::Get(void) const
{
  return codec_.get();
}

}