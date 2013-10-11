#include "decoder.h"
#include "decoder_impl.h"
#include "ffmpeg_manager.h"

#include <queue>

namespace audio
{

DecoderImpl::DecoderImpl(std::string filename)
{
  ffmpeg::init();
  format_ = ffmpeg::Format(filename);
  codec_ = ffmpeg::Codec(format_);
  resampler_ = ffmpeg::Resampler(codec_);

  packets_finished_ = false;

  total_buffer_size_ = 0;
  target_buffer_size_ = 1 << 20;

  replenish_buffer();
  remaining_current_buffer_size_ = queue_.front().size();
  data_ = queue_.front().data()[0];
}

DecoderImpl::~DecoderImpl(void)
{
  if(codec_->codec->capabilities & CODEC_CAP_DELAY)
  {
    ffmpeg::Packet packet;
    int frameFinished = 0;
    while(avcodec_decode_audio4(codec_.get(), frame_.get(), &frameFinished, packet.get()) >= 0 && frameFinished)
    {
      av_frame_unref(frame_.get());
    }
  }
}

void DecoderImpl::decode(void)
{
  if(!packets_finished_) 
  {
    ffmpeg::Packet packet;
    if(av_read_frame(format_.format(), packet.get()) == 0)
    {
      if(packet->stream_index == format_.audio_stream()->index)
      {
        int frame_finished = 0;
        avcodec_decode_audio4(codec_.get(), frame_.get(), &frame_finished, packet.get());

        if(frame_finished)
        {
          ffmpeg::Samples samples = resampler_.resample((uint8_t const**)frame_->data, frame_->nb_samples);
          total_buffer_size_ += samples.size();
          queue_.push(samples);
          av_frame_unref(frame_.get());
        }
      }
    }
    else
    {
      packets_finished_ = true;
    }
  }
}

void DecoderImpl::replenish_buffer(void)
{
  while((total_buffer_size_ < target_buffer_size_) && !packets_finished_)
  {
    decode();
  }
}

bool DecoderImpl::empty(void)
{
  return queue_.empty();
}

void DecoderImpl::read(uint8_t* buffer, int size)
{
  replenish_buffer();

  while(size)
  {
    if(!queue_.empty())
    {
      if(size < remaining_current_buffer_size_)
      {
        memcpy(buffer, data_, size);
        data_ += size;
        remaining_current_buffer_size_ -= size;
        total_buffer_size_ -= size;
      }
      else
      {
        memcpy(buffer, data_, remaining_current_buffer_size_);
        buffer += remaining_current_buffer_size_;
        total_buffer_size_ -= remaining_current_buffer_size_;
        size -= remaining_current_buffer_size_;
        queue_.pop();
        if(!queue_.empty())
        {
          data_ = queue_.front().data()[0];
          remaining_current_buffer_size_ = queue_.front().size();
        }
        else
        {
          replenish_buffer();
        }
      }
    }
    else
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

void Decoder::read(uint8_t* buffer, int size)
{
  impl_->read(buffer, size);
}

bool Decoder::empty(void)
{
  return impl_->empty();
}

}