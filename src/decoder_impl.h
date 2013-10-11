#ifndef DECODER_IMPL_H_
#define DECODER_IMPL_H_

#include <stdint.h>
#include <queue>
#include <string>
#include "format.h"
#include "codec.h"
#include "resampler.h"
#include "samples.h"
#include "frame.h"
#include "packet.h"

namespace audio
{

class DecoderImpl
{
public:
  ffmpeg::Format format_;
  ffmpeg::Codec codec_;
  ffmpeg::Resampler resampler_;
  ffmpeg::Frame frame_;

  bool packets_finished_;

  std::queue<ffmpeg::Samples> queue_;
  uint8_t* data_;
  int total_buffer_size_;
  int remaining_current_buffer_size_;
  int target_buffer_size_;

  DecoderImpl(std::string filename);
  ~DecoderImpl(void);

  void decode(void);
  void replenish_buffer(void);
  bool empty(void);
  void read(uint8_t* buffer, int size);
};

}

#endif