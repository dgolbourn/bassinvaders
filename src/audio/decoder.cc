#include "decoder.h"
#include "decoder_impl.h"
#include "ffmpeg_manager.h"
#include "buffer.h"
#include "packet.h"

#include <queue>

namespace audio
{

DecoderImpl::DecoderImpl(std::string filename)
{
  ffmpeg::Init();
  format_ = ffmpeg::Format(filename);
  codec_ = ffmpeg::Codec(format_);
  resampler_ = ffmpeg::Resampler(codec_);
  buffer_ = ffmpeg::Buffer(1 << 20);

  packets_finished_ = false;
  ReplenishBuffer();
}

DecoderImpl::~DecoderImpl(void)
{
  if(codec_->codec->capabilities & CODEC_CAP_DELAY)
  {
    ffmpeg::Packet packet;
    int frameFinished = 0;
    while(avcodec_decode_audio4(codec_.Get(), frame_.Get(), &frameFinished, packet.Get()) >= 0 && frameFinished)
    {
      av_frame_unref(frame_.Get());
    }
  }
}

void DecoderImpl::Decode(void)
{
  if(!packets_finished_) 
  {
    ffmpeg::Packet packet;
    if(av_read_frame(format_.format(), packet.Get()) == 0)
    {
      if(packet->stream_index == format_.audio_stream()->index)
      {
        int frame_finished = 0;
        avcodec_decode_audio4(codec_.Get(), frame_.Get(), &frame_finished, packet.Get());

        if(frame_finished)
        {
          ffmpeg::Samples samples = resampler_.Resample((uint8_t const**)frame_->data, frame_->nb_samples);
          buffer_.Add(samples);
          av_frame_unref(frame_.Get());
        }
      }
    }
    else
    {
      packets_finished_ = true;
    }
  }
}

void DecoderImpl::ReplenishBuffer(void)
{
  while(!buffer_.Full() && !packets_finished_)
  {
    Decode();
  }
}

bool DecoderImpl::Empty(void)
{
  return buffer_.Empty() && packets_finished_;
}

void DecoderImpl::Read(uint8_t* buffer, int size)
{
  while(size)
  {
    ReplenishBuffer();
    int read_size = buffer_.Read(buffer, size);
    buffer += read_size;
    size -= read_size;

    if(Empty())
    {
      memset(buffer, 0, size);
      size = 0;
    }
  }
}

Decoder::Decoder(void)
{
}

Decoder::Decoder(std::string filename)
{
  impl_ = std::shared_ptr<DecoderImpl>(new DecoderImpl(filename));
}

Decoder::Decoder(const Decoder& other)
{
  impl_ = other.impl_;
}

Decoder::Decoder(Decoder&& other)
{
  impl_ = other.impl_;
  other.impl_.reset();
}

Decoder& Decoder::operator=(Decoder other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Decoder::~Decoder(void)
{
}

void Decoder::Read(uint8_t* buffer, int size)
{
  impl_->Read(buffer, size);
}

bool Decoder::Empty(void)
{
  return impl_->Empty();
}

}