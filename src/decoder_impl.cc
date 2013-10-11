#include "decoder_impl.h"
#include "ffmpeg_manager.h"

namespace audio
{

DecoderImpl::DecoderImpl(std::string filename)
{
  ffmpeg::init();
  format_ = Format(filename);
  codec_ = Codec(format_);
  resampler_ = Resampler(codec_);

  packets_finished_ = false;

  total_buffer_size_ = 0;
  target_buffer_size_ = 1 << 20;

  replenish_buffer();
  remaining_current_buffer_size_ = queue_.front().impl_->size_;
  data_ = queue_.front().impl_->data_[0];
}

DecoderImpl::~DecoderImpl(void)
{
  if(codec_.codec_->codec->capabilities & CODEC_CAP_DELAY)
  {
    Packet packet;
    int frameFinished = 0;
    while(avcodec_decode_audio4(codec_.codec_.get(), frame_.frame_.get(), &frameFinished, packet.packet_.get()) >= 0 && frameFinished)
    {
      av_frame_unref(frame_.frame_.get());
    }
  }
}

void DecoderImpl::decode(void)
{
  if(!packets_finished_) 
  {
    Packet packet;
    if(av_read_frame(format_.impl_->format_, packet.packet_.get()) == 0)
    {
      if(packet.packet_->stream_index == format_.impl_->audio_stream_->index)
      {
        int frame_finished = 0;
        avcodec_decode_audio4(codec_.codec_.get(), frame_.frame_.get(), &frame_finished, packet.packet_.get());

        if(frame_finished)
        {
          Samples samples = resampler_.resample((uint8_t const**)frame_.frame_->data, frame_.frame_->nb_samples);
          total_buffer_size_ += samples.impl_->size_;
          queue_.push(samples);
          av_frame_unref(frame_.frame_.get());
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
          data_ = queue_.front().impl_->data_[0];
          remaining_current_buffer_size_ = queue_.front().impl_->size_;
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

}