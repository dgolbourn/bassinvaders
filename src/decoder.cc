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

class FormatContextImpl
{
public:
  AVFormatContext* format_context_;
  AVStream* audio_stream_;

  FormatContextImpl(std::string filename)
  {
    format_context_ = avformat_alloc_context();
    if(!format_context_)
    {
      throw ffmpeg::Exception();
    }
    if(avformat_open_input(&format_context_, filename.c_str(), nullptr, nullptr))
    {
      throw ffmpeg::Exception();
    }
    if(avformat_find_stream_info(format_context_, nullptr) < 0)
    {
      throw ffmpeg::Exception();
    }

    audio_stream_ = nullptr;
    for(unsigned int i = 0; i < format_context_->nb_streams; ++i)
    {
      if (format_context_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
      {
        audio_stream_ = format_context_->streams[i];
        break;
      }
    }
    if(!audio_stream_)
    {
      throw ffmpeg::Exception();
    }
  }

  ~FormatContextImpl(void)
  {
    if(format_context_)
    {
      avformat_close_input(&format_context_);
      avformat_free_context(format_context_);
    }
  }
};

class FormatContext
{
public:
  std::shared_ptr<FormatContextImpl> impl_;

  FormatContext(void)
  {    
  }

  FormatContext(std::string filename)
  {
    impl_ = std::shared_ptr<FormatContextImpl>(new FormatContextImpl(filename));
  }

  FormatContext(const FormatContext& other)
  {
    impl_ = other.impl_;
  }

  FormatContext(FormatContext&& other)
  {
    impl_ = other.impl_;
    other.impl_.reset();
  }

  FormatContext& operator=(FormatContext other)
  {
    std::swap(impl_, other.impl_);
    return *this;
  }

  ~FormatContext(void)
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

  CodecContext(FormatContext& format_context)
  {
    AVCodecContext* codec_context = format_context.impl_->audio_stream_->codec;
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

class SamplesImpl
{
public:
  uint8_t** data_;
  int size_;

  SamplesImpl(uint8_t** data, int size)
  {
    data_ = data;
    size_ = size;
  }

  SamplesImpl(int channels, int size, AVSampleFormat format)
  {
    if(av_samples_alloc_array_and_samples(&data_, nullptr, channels, size, format, 0) < 0)
    {
      data_ = nullptr;
      throw ffmpeg::Exception();
    }
    size_ = size;
  }

  ~SamplesImpl(void)
  {
    if(data_)
    {
      av_freep(&data_[0]);
      av_freep(data_);
    }
  }
};

class Samples
{
public:
  std::shared_ptr<SamplesImpl> impl_;

  Samples(uint8_t** data, int size)
  {
    impl_ = std::shared_ptr<SamplesImpl>(new SamplesImpl(data, size));
  }

  Samples(void)
  {
  }

  Samples(int channels, int size, AVSampleFormat format)
  {
    impl_ = std::shared_ptr<SamplesImpl>(new SamplesImpl(channels, size, format));
  }

  Samples(const Samples& other)
  {
    impl_ = other.impl_;
  }

  Samples(Samples&& other)
  {
    impl_ = other.impl_;
    other.impl_.reset();
  }

  Samples& operator=(Samples other)
  {
    std::swap(impl_, other.impl_);
    return *this;
  }

  ~Samples(void)
  {
  }
};

void handle_output(uint8_t** output, int out_samples)
{
  for(int i = 0; i < out_samples; i+=2)
  {
    std::cout << *((int16_t*)&output[0][i]) << " ";
  }
}

class ResamplerImpl
{
public:
  SwrContext* swr_;
  int input_sample_rate_;
  int output_channels_;
  int output_sample_rate_;
  AVSampleFormat output_format_;
  uint64_t output_channel_layout_;

  ResamplerImpl(CodecContext& codec_context)
  {
    swr_ = swr_alloc();
    if(!swr_)
    {
      throw ffmpeg::Exception();
    }

    output_sample_rate_ = 22050;
    output_format_ = AV_SAMPLE_FMT_S16;
    output_channel_layout_ = AV_CH_LAYOUT_STEREO;

    av_opt_set_int(swr_, "in_channel_layout", codec_context.codec_context_->channel_layout, 0);
    av_opt_set_int(swr_, "out_channel_layout", output_channel_layout_, 0);
    av_opt_set_int(swr_, "in_sample_rate", codec_context.codec_context_->sample_rate, 0);
    av_opt_set_int(swr_, "out_sample_rate", output_sample_rate_, 0);
    av_opt_set_sample_fmt(swr_, "in_sample_fmt", codec_context.codec_context_->sample_fmt, 0);
    av_opt_set_sample_fmt(swr_, "out_sample_fmt", output_format_, 0);
    
    if(swr_init(swr_))
    {
      throw ffmpeg::Exception();
    }

    input_sample_rate_ = codec_context.codec_context_->sample_rate;
    output_channels_ =  av_get_channel_layout_nb_channels(output_channel_layout_);
  }

  ~ResamplerImpl(void)
  {
    if(swr_)
    {
      swr_free(&swr_);
    }
  }

  Samples resample(uint8_t const** input, int in_samples)
  {
    uint8_t** output;
    int64_t delay = swr_get_delay(swr_, input_sample_rate_) + in_samples;
    int out_samples = (int)av_rescale_rnd(delay, output_sample_rate_, input_sample_rate_, AV_ROUND_UP);    
    int linesize;
    if(av_samples_alloc_array_and_samples(&output, &linesize, output_channels_, out_samples, output_format_, 1) < 0)
    {
      throw ffmpeg::Exception();
    }
    int conv_samples = swr_convert(swr_, output, out_samples, input, in_samples);
    if(conv_samples < 0)
    {
      throw ffmpeg::Exception();
    }
    int size = av_samples_get_buffer_size(&linesize, output_channels_, conv_samples, output_format_, 1);
    return Samples(output, size);
  }
};

class Resampler
{
  std::shared_ptr<ResamplerImpl> impl_;
public:

  Resampler(void)
  {
  }

  Resampler(CodecContext& codec_context)
  {
    impl_ = std::shared_ptr<ResamplerImpl>(new ResamplerImpl(codec_context));
  }
  
  Resampler(const Resampler& other)
  {
    impl_ = other.impl_;
  }

  Resampler(Resampler&& other)
  {
    impl_ = other.impl_;
    other.impl_.reset();
  }

  Resampler& operator=(Resampler other)
  {
    std::swap(impl_, other.impl_);
    return *this;
  }

  ~Resampler(void)
  {
  }

  Samples resample(uint8_t const** input, int in_samples)
  {
    return impl_->resample(input, in_samples);
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
  CodecContext codec_context_;
  Resampler resampler_;
  Frame frame_;
  bool packets_finished_;

  DecoderImpl(std::string filename)
  {
    ffmpeg::init();
    format_context_ = FormatContext(filename);
    codec_context_ = CodecContext(format_context_);
    resampler_ = Resampler(codec_context_);
    packets_finished_ = false;
  }

  ~DecoderImpl(void)
  {
    if(codec_context_.codec_context_->codec->capabilities & CODEC_CAP_DELAY)
    {
      Packet packet;
      int frameFinished = 0;
      while (avcodec_decode_audio4(codec_context_.codec_context_.get(), frame_.frame_.get(), &frameFinished, packet.packet_.get()) >= 0 && frameFinished)
      {
        av_frame_unref(frame_.frame_.get());
      }
    }
  }

  void decode(void)
  {
    while(!packets_finished_) 
    {
      Packet packet;
      if(av_read_frame(format_context_.impl_->format_context_, packet.packet_.get()) == 0)
      {
        if(packet.packet_->stream_index == format_context_.impl_->audio_stream_->index)
        {
          int frame_finished = 0;
          avcodec_decode_audio4(codec_context_.codec_context_.get(), frame_.frame_.get(), &frame_finished, packet.packet_.get());

          if(frame_finished)
          {
            Samples samples = resampler_.resample((uint8_t const**)frame_.frame_->data, frame_.frame_->nb_samples);
            handle_output(samples.impl_->data_, samples.impl_->size_);
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
};

class Decoder
{
  std::shared_ptr<DecoderImpl> impl_;
public:
  Decoder(void)
  {
  }

  Decoder(std::string filename)
  {
    impl_ = std::shared_ptr<DecoderImpl>(new DecoderImpl(filename));
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