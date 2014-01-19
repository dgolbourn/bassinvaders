#include "decoder.h"
#include "ffmpeg_library.h"
#include "packet.h"
#include "format.h"
#include "codec.h"
#include "frame.h"
#include "ffmpeg_exception.h"
#include "filter.h"
#include "cstd_exception.h"
#include "audio_format.h"

namespace ffmpeg
{
class DecoderImpl
{
public:
  DecoderImpl(std::string const& filename);
  void Read(uint8_t* buffer, int size);
  void Volume(double volume);
  ~DecoderImpl(void);
  Library const ffmpeg_;
  Format format_;
  Codec codec_;
  Frame frame_;
  Filter filter_;
  uint8_t* buffer_;
  uint8_t* buffer_ptr_;
  int buffer_size_;
  bool decode_complete_;
  bool empty_;
};

DecoderImpl::DecoderImpl(std::string const& filename)
{
  decode_complete_ = false;
  empty_ = false;
  buffer_ = nullptr;
  buffer_ptr_ = nullptr;
  buffer_size_ = 0;
  format_ = Format(filename);
  codec_ = Codec(format_);
  filter_ = Filter(format_, codec_);
}

static Packet ReadAudio(Format const& format)
{
  bool audio_read = false;
  Packet packet;
  if(av_read_frame(format, packet) == 0)
  {
    if(packet->stream_index == format.AudioStream()->index)
    {
      audio_read = true;
    }
  }
  if(!audio_read)
  {
    packet = Packet();
  }
  return packet;
}

static bool DecodeAudio(Codec const& codec, Frame const& frame, Packet& packet)
{
  int frame_finished;
  int amount = avcodec_decode_audio4(codec, frame, &frame_finished, packet);
  if(amount < 0)
  {
    throw Exception();
  }
  packet += amount;
  return frame_finished != 0;
}

static void Copy(uint8_t* destination, uint8_t* source, int size)
{
  if(!memcpy(destination, source, size))
  {
    throw cstd::Exception();
  }
}

void DecoderImpl::Read(uint8_t* buffer, int size)
{
  while(size)
  {
    if(buffer_ptr_)
    {
      if(buffer_size_ <= size)
      {
        Copy(buffer, buffer_ptr_, buffer_size_);
        buffer += buffer_size_;
        size -= buffer_size_;
        buffer_size_ = 0;
        free(buffer_);
        buffer_ = nullptr;
        buffer_ptr_ = nullptr;
      }
      else
      {
        Copy(buffer, buffer_ptr_, size);
        buffer += size;
        buffer_ptr_ += size;
        buffer_size_ -= size;
        size = 0;
      }
    }
    else
    {
      while(filter_.Read(frame_))
      {
        int frame_size = frame_->nb_samples * av_get_channel_layout_nb_channels(FFMPEG_CHANNEL_LAYOUT) * av_get_bytes_per_sample(FFMPEG_FORMAT);
        if(frame_size <= size)
        {
          Copy(buffer, frame_->data[0], frame_size);
          buffer += frame_size;
          size -= frame_size;
        }
        else
        {
          Copy(buffer, frame_->data[0], size);
          buffer += size;
          buffer_size_ = frame_size - size;
          buffer_ = (uint8_t*)malloc(buffer_size_);
          if(!buffer_)
          {
            throw cstd::Exception();
          }
          buffer_ptr_ = buffer_;
          Copy(buffer_, frame_->data[0] + size, buffer_size_);
          size = 0;
        }
        frame_.Clear();
      }
      if(!decode_complete_)
      {
        if(Packet packet = ReadAudio(format_))
        {
          while(packet)
          {
            if(DecodeAudio(codec_, frame_, packet))
            {
              filter_.Add(frame_);
              frame_.Clear();
            }
          }
        }
        else
        {
          decode_complete_ = true;
        }
      }
      else
      {
        if(!memset(buffer, 0, size))
        {
          throw cstd::Exception();
        }
        size = 0;
        empty_ = true;
      }
    }
  }
}

void DecoderImpl::Volume(double volume)
{
  filter_.Volume(volume);
}

DecoderImpl::~DecoderImpl(void)
{
  if(buffer_)
  {
    free(buffer_);
  }
}

Decoder::Decoder(std::string const& filename)
{
  impl_ = std::make_shared<DecoderImpl>(filename);
}

void Decoder::Read(uint8_t* buffer, int size)
{
  impl_->Read(buffer, size);
}

Decoder::operator bool(void) const
{
  return !impl_->empty_;
}

void Decoder::Volume(double volume)
{
  impl_->Volume(volume);
}
}