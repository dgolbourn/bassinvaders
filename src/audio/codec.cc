#include "codec.h"
#include "ffmpeg_exception.h"
#include "packet.h"

namespace ffmpeg
{
class CodecImpl
{
public:
  CodecImpl(Format const& format);
  bool Read(Frame& frame);

  ~CodecImpl(void);

  AVCodecContext* context_;
  int stream_;
};

CodecImpl::CodecImpl(Format const& format)
{
  context_ = nullptr;
  if(avformat_find_stream_info(format, nullptr) < 0)
  {
    throw Exception();
  }
  AVCodec* codec;
  stream_ = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
  if(stream_ < 0)
  {
    throw Exception();
  }
  context_ = format->streams[stream_]->codec;
  if(avcodec_open2(context_, codec, nullptr))
  {
    throw Exception();
  }
}

CodecImpl::~CodecImpl(void)
{
  avcodec_close(context_);
}

bool CodecImpl::Read(Frame& frame)
{
  Packet packet;
  int frame_read = 0;
  while(!frame_read)
  {
    int amount = avcodec_decode_audio4(context_, frame, &frame_read, packet);
    if(amount == 0)
    {
      break;
    }
    if(amount < 0)
    {
      throw Exception();
    }
  }
  return frame_read != 0;
}

Codec::Codec(Format const& format)
{
  impl_ = std::make_shared<CodecImpl>(format);
}

AVCodecContext* Codec::operator->(void) const
{
  return impl_->context_;
}

Codec::operator AVCodecContext*(void) const
{
  return impl_->context_;
}

int Codec::Stream(void) const
{
  return impl_->stream_;
}

bool Codec::Read(Frame& frame)
{
  return impl_->Read(frame);
}
}