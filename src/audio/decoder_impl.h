#ifndef DECODER_IMPL_H_
#define DECODER_IMPL_H_

#include <stdint.h>
#include <string>
#include "format.h"
#include "codec.h"
#include "resampler.h"
#include "frame.h"
#include "buffer.h"

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

  DecoderImpl(std::string filename);
  ~DecoderImpl(void);

  void Decode(void);
  void ReplenishBuffer(void);
  bool Empty(void);
  void Read(uint8_t* buffer, int size);
};

}

#endif