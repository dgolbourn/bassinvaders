#ifndef SAMPLES_IMPL_H_
#define SAMPLES_IMPL_H_

extern "C"
{
#include "libavutil/samplefmt.h"
}
#include <stdint.h>

namespace ffmpeg
{

class SamplesImpl
{
public:
  uint8_t** data_;
  int size_;

  SamplesImpl(uint8_t** data, int size);
  SamplesImpl(int channels, int size, AVSampleFormat format);
  ~SamplesImpl(void);
};

}

#endif