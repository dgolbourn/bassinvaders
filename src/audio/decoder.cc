#include "decoder.h"
#include "ffmpeg_manager.h"
#include "buffer.h"
#include "packet.h"
#include "format.h"
#include "codec.h"
#include "resampler.h"
#include "frame.h"
#include "ffmpeg_exception.h"

#include <queue>
#include <thread>

namespace audio
{

class DecoderImpl
{
public:
  ffmpeg::Format format_;
  ffmpeg::Codec codec_;
  ffmpeg::Resampler resampler_;
  ffmpeg::Frame frame_;
  ffmpeg::Buffer buffer_;

  bool packets_finished_;

  std::thread* thread_;

  DecoderImpl(std::string& filename, int buffer_size);
  ~DecoderImpl(void);

  void Decode(void);
  void ReplenishBuffer(void);
  bool Empty(void);
  void Read(uint8_t* buffer, int size);
  void Flush(void);
};

static void ReplenishBufferThread(DecoderImpl* impl)
{
  while(!impl->packets_finished_)
  {
    impl->ReplenishBuffer();
  }
}

DecoderImpl::DecoderImpl(std::string& filename, int buffer_size)
{
  ffmpeg::Init();
  format_ = ffmpeg::Format(filename);
  codec_ = ffmpeg::Codec(format_);
  resampler_ = ffmpeg::Resampler(codec_);
  buffer_ = ffmpeg::Buffer(buffer_size);

  packets_finished_ = false;
  ReplenishBuffer();
  thread_ = new std::thread(ReplenishBufferThread, this);
}

DecoderImpl::~DecoderImpl(void)
{
  packets_finished_ = true;
  thread_->join();
  Flush();
}

void DecoderImpl::Flush(void)
{
  if(codec_->codec->capabilities & CODEC_CAP_DELAY)
  {
    ffmpeg::Packet packet;
    int frameFinished = 0;
    while(avcodec_decode_audio4(codec_.Get(), frame_.Get(), &frameFinished, packet.Get()) >= 0 && frameFinished)
    {
      frame_.Clear();
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
        while(!packet.Empty())
        {
          int frame_finished = 0;
          int amount = avcodec_decode_audio4(codec_.Get(), frame_.Get(), &frame_finished, packet.Get());
          if(amount < 0)
          { 
            throw ffmpeg::Exception();
          }
          if(frame_finished)
          {
            ffmpeg::Samples samples = resampler_.Resample((uint8_t const**)frame_->data, frame_->nb_samples);
            buffer_.Add(samples);
            frame_.Clear();
          }
          packet.Next(amount);
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

Decoder::Decoder(std::string& filename, int buffer_size) : impl_(new DecoderImpl(filename, buffer_size))
{
}

Decoder::Decoder(Decoder const& other) : impl_(other.impl_)
{
}

Decoder::Decoder(Decoder&& other) : impl_(std::move(other.impl_))
{
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