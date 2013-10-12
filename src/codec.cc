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

Codec::Codec(void)
{    
}

Codec::Codec(Format& format)
{
  AVCodecContext* codec = format.audio_stream()->codec;
  codec->codec = avcodec_find_decoder(codec->codec_id);
  if(!codec->codec)
  {
    throw ffmpeg::Exception();
  }
  if(avcodec_open2(codec, codec->codec, nullptr))
  {
    throw ffmpeg::Exception();
  }
  codec_ = std::shared_ptr<AVCodecContext>(codec, Deleter());
}

Codec::Codec(const Codec& other)
{
  codec_ = other.codec_;
}

Codec::Codec(Codec&& other)
{
  codec_ = other.codec_;
  other.codec_.reset();
}

Codec& Codec::operator=(Codec other)
{
  std::swap(codec_, other.codec_);
  return *this;
}

Codec::~Codec(void)
{
}

AVCodecContext* Codec::operator->(void)
{
  return codec_.operator->();
}

AVCodecContext* Codec::Get(void)
{
  return codec_.get();
}

}