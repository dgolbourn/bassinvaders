#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

#include <string>
#include <memory>

#include "ffmpeg_manager.h"
#include "ffmpeg_exception.h"

class Frame
{
public:
  std::shared_ptr<AVFrame> frame_;

  class Deleter
  {
  public:
    void operator()(AVFrame* frame)
    {
      av_free(frame);
    }
  };

  Frame(void)
  {
    AVFrame* frame = avcodec_alloc_frame();
    if(!frame)
    {
      throw ffmpeg::Exception();
    }
    frame_ = std::shared_ptr<AVFrame>(frame, Deleter());
  }

  Frame(const Frame& other)
  {
    frame_ = other.frame_;
  }

  Frame(Frame&& other)
  {
    frame_ = other.frame_;
    other.frame_.reset();
  }

  Frame& operator=(Frame other)
  {
    std::swap(frame_, other.frame_);
    return *this;
  }

  ~Frame(void)
  {
  }
};

class FormatContext
{
public:
  std::shared_ptr<AVFormatContext> format_context_;

  class Deleter
  {
  public:
    void operator()(AVFormatContext* format_context)
    {
      avformat_close_input(&format_context);
      avformat_free_context(format_context);
    }
  };

  FormatContext(void)
  {    
  }

  FormatContext(std::string filename)
  {
    AVFormatContext* format_context;
    format_context = avformat_alloc_context();
    if(!format_context)
    {
      throw ffmpeg::Exception();
    }
    format_context_ = std::shared_ptr<AVFormatContext>(format_context, Deleter());
    if(avformat_open_input(&format_context, filename.c_str(), nullptr, nullptr))
    {
      throw ffmpeg::Exception();
    }
    if(avformat_find_stream_info(format_context, nullptr) < 0)
    {
      throw ffmpeg::Exception();
    }
  }

  FormatContext(const FormatContext& other)
  {
    format_context_ = other.format_context_;
  }

  FormatContext(FormatContext&& other)
  {
    format_context_ = other.format_context_;
    other.format_context_.reset();
  }

  FormatContext& operator=(FormatContext other)
  {
    std::swap(format_context_, other.format_context_);
    return *this;
  }

  ~FormatContext(void)
  {
  }
};

class AudioStream
{
public:
  std::shared_ptr<AVStream> audio_stream_;

  AudioStream(void)
  {    
  }

  AudioStream(FormatContext& format_context)
  {
    AVStream* audio_stream = nullptr;
    for(unsigned int i = 0; i < format_context.format_context_->nb_streams; ++i)
    {
      if (format_context.format_context_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
      {
        audio_stream = format_context.format_context_->streams[i];
        break;
      }
    }

    if(!audio_stream)
    {
      throw ffmpeg::Exception();
    }
    audio_stream_ = std::shared_ptr<AVStream>(audio_stream);
  }

  AudioStream(const AudioStream& other)
  {
    audio_stream_ = other.audio_stream_;
  }

  AudioStream(AudioStream&& other)
  {
    audio_stream_ = other.audio_stream_;
    other.audio_stream_.reset();
  }

  AudioStream& operator=(AudioStream other)
  {
    std::swap(audio_stream_, other.audio_stream_);
    return *this;
  }

  ~AudioStream(void)
  {
  }
};

class CodecContext
{
public:
  std::shared_ptr<AVCodecContext> codec_context_;

  class Deleter
  {
  public:
    void operator()(AVCodecContext* codec_context)
    {
      avcodec_close(codec_context);
    }
  };

  CodecContext(void)
  {    
  }

  CodecContext(AudioStream& audio_stream)
  {
    AVCodecContext* codec_context = audio_stream.audio_stream_->codec;
    codec_context->codec = avcodec_find_decoder(codec_context->codec_id);
    if(!codec_context->codec)
    {
      throw ffmpeg::Exception();
    }
    if(avcodec_open2(codec_context, codec_context->codec, nullptr))
    {
      throw ffmpeg::Exception();
    }
    codec_context_ = std::shared_ptr<AVCodecContext>(codec_context, Deleter());
  }

  CodecContext(const CodecContext& other)
  {
    codec_context_ = other.codec_context_;
  }

  CodecContext(CodecContext&& other)
  {
    codec_context_ = other.codec_context_;
    other.codec_context_.reset();
  }

  CodecContext& operator=(CodecContext other)
  {
    std::swap(codec_context_, other.codec_context_);
    return *this;
  }

  ~CodecContext(void)
  {
  }
};

class Resample
{
  std::shared_ptr<SwrContext> swr_;
  int sample_rate_;
public:
  class Deleter
  {
  public:
    void operator()(SwrContext* swr)
    {
      swr_free(&swr);
    }
  };

  Resample(void)
  {
  }

  Resample(CodecContext& codec_context)
  {
    SwrContext* swr = swr_alloc();
    if(!swr)
    {
      throw ffmpeg::Exception();
    }
    swr_ = std::shared_ptr<SwrContext>(swr, Deleter());

    av_opt_set_int(swr, "in_channel_layout", codec_context.codec_context_->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "in_sample_rate", codec_context.codec_context_->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", 22050, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", codec_context.codec_context_->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    
    if(swr_init(swr))
    {
      throw ffmpeg::Exception();
    }

    sample_rate_ = codec_context.codec_context_->sample_rate;
  }
  
  Resample(const Resample& other)
  {
    swr_ = other.swr_;
    sample_rate_ = other.sample_rate_;
  }

  Resample(Resample&& other)
  {
    swr_ = other.swr_;
    sample_rate_ = other.sample_rate_;
    other.swr_.reset();
  }

  Resample& operator=(Resample other)
  {
    std::swap(swr_, other.swr_);
    std::swap(sample_rate_, other.sample_rate_);
    return *this;
  }

  ~Resample(void)
  {
  }

  void handle_output(uint8_t** output, int out_samples)
  {
    for(int i = 0; i < out_samples; i+=2)
    {
      std::cout << *((int16_t*)&output[0][i]) << " ";
    }
  }

  void resample(uint8_t const** input, int in_samples)
  {
    uint8_t** output; 
    int64_t delay = swr_get_delay(swr_.get(), sample_rate_) + in_samples;
    int out_samples = (int)av_rescale_rnd(delay, 22050, sample_rate_, AV_ROUND_UP);    
    av_samples_alloc_array_and_samples(&output, nullptr, 2, out_samples, AV_SAMPLE_FMT_S16, 0);
    out_samples = swr_convert(swr_.get(), output, out_samples, input, in_samples);
    handle_output(output, out_samples);    
    av_freep(output);
  }
};

class Packet
{
public:
  std::shared_ptr<AVPacket> packet_;

  class Deleter
  {
  public:
    void operator()(AVPacket* packet)
    {
      av_free_packet(packet);
      free(packet);
    }
  };

  Packet(void)
  {
    AVPacket* packet = (AVPacket*)malloc(sizeof(AVPacket));
    if(!packet)
    {
      throw ffmpeg::Exception();
    }
    packet_ = std::shared_ptr<AVPacket>(packet, Deleter());
    av_init_packet(packet);
  }

  Packet(const Packet& other)
  {
    packet_ = other.packet_;
  }

  Packet(Packet&& other)
  {
    packet_ = other.packet_;
    other.packet_.reset();
  }

  Packet& operator=(Packet other)
  {
    std::swap(packet_, other.packet_);
    return *this;
  }

  ~Packet(void)
  {
  }
};

class DecoderImpl
{
public:
  FormatContext format_context_;
  AudioStream audio_stream_;
  CodecContext codec_context_;
  Resample resample_;
  Frame frame_;

  DecoderImpl(std::string filename)
  {
    ffmpeg::init();
    format_context_ = FormatContext(filename);
    audio_stream_ = AudioStream(format_context_);
    codec_context_ = CodecContext(audio_stream_);
    resample_ = Resample(codec_context_);
  }

  void decode(void)
  {
    bool packets_finished = false;
    while(!packets_finished) 
    {
      Packet packet;
      if(av_read_frame(format_context_.format_context_.get(), packet.packet_.get()) == 0)
      {
        if (packet.packet_->stream_index == audio_stream_.audio_stream_->index)
        {
          int frame_finished = 0;
          avcodec_decode_audio4(codec_context_.codec_context_.get(), frame_.frame_.get(), &frame_finished, packet.packet_.get());

          if(frame_finished)
          {
            resample_.resample((uint8_t const**)frame_.frame_->data, frame_.frame_->nb_samples);
          }
        }
      }
      else
      {
        packets_finished = true;
      }
    }

    if (codec_context_.codec_context_->codec->capabilities & CODEC_CAP_DELAY)
    {
      Packet packet;
      int frameFinished = 0;
      while (avcodec_decode_audio4(codec_context_.codec_context_.get(), frame_.frame_.get(), &frameFinished, packet.packet_.get()) >= 0 && frameFinished)
      {
      }
    }
  }
};

class Decoder
{
  std::shared_ptr<DecoderImpl> impl_;

  class Deleter
  {
  public:
    void operator()(DecoderImpl* impl)
    {
      delete impl;
    }
  };

public:
  Decoder(void)
  {
  }

  Decoder(std::string filename)
  {
    DecoderImpl* impl = new DecoderImpl(filename);
    if(!impl)
    {
      throw ffmpeg::Exception();
    }
    impl_ = std::shared_ptr<DecoderImpl>(impl, Deleter());
  }

  Decoder(const Decoder& other)
  {
    impl_ = other.impl_;
  }

  Decoder(Decoder&& other)
  {
    impl_ = other.impl_;
    other.impl_.reset();
  }

  Decoder& operator=(Decoder other)
  {
    std::swap(impl_, other.impl_);
    return *this;
  }

  ~Decoder(void)
  {
  }

  void decode(void)
  {
    impl_->decode();
  }
};

int main()
{
  Decoder decoder("16Hz-20kHz-Exp-1f-10sec.mp3");
  decoder.decode();
}